/* C:B**************************************************************************
 This software is © 2014 Bright Plaza Inc. <drivetrust@drivetrust.com>

 This file is part of sedutil.

 sedutil is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 sedutil is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with sedutil.  If not, see <http://www.gnu.org/licenses/>.

 * C:E********************************************************************** */

#include "log.h"

#include "DtaWindows.h"
#include "DtaHexDump.h"
#include "NVMeStructures.h"
#pragma warning(push)
#pragma warning(disable : 4091)
#include <Ntddscsi.h>
#pragma warning(pop)
//#include <winioctl.h>


const std::string DtaOS::name="Windows";
DtaOS * DtaOS::getDtaOS () { return new DtaWindows(); }

OSDEVICEHANDLE DtaWindows::openDeviceHandle(const char * devref, bool & accessDenied){
    LOG(D4) << "openDeviceHandle(\"" << devref << "\", _)";
    OSDEVICEHANDLE osDeviceHandle = (OSDEVICEHANDLE)CreateFile(
        devref,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (INVALID_HANDLE_VALUE != osDeviceHandle)
        LOG(D4) << "Opened device handle " << HEXON(2) << (size_t)osDeviceHandle << " for " << devref;
    else {
        LOG(D4) << "Failed to open device handle for " << devref;
        accessDenied = (ERROR_ACCESS_DENIED == GetLastError());
    }

    if (accessDenied) {
        closeDeviceHandle(osDeviceHandle);
        return INVALID_HANDLE_VALUE;
    }

    return osDeviceHandle;
}

void DtaWindows::closeDeviceHandle(OSDEVICEHANDLE osDeviceHandle){
    if (osDeviceHandle == INVALID_HANDLE_VALUE) return;
    LOG(D4) << "Closing device handle " << HEXON(2) << (size_t)osDeviceHandle;
    (void)CloseHandle((HANDLE)osDeviceHandle);
    LOG(D4) << "Closed device handle";
}


std::vector<std::string> DtaWindows::generateDtaDriveDevRefs()
{
    std::vector<std::string> devrefs;
    for (int i = 0; i < MAX_DISKS; i++)
        devrefs.push_back(std::string("\\\\.\\PhysicalDrive") + std::to_string(i));
    return devrefs;
}

void DtaWindows::errorNoAccess(const char* devref) {
    if (devref == NULL) {
        LOG(E) << "You do not have permission to access the raw disk(s) in write mode";
    } else {
        LOG(E) << "You do not have permission to access the raw disk " << devref << " in write mode";
    }
    LOG(E) << "Perhaps you might try run as an administrator";
}





DtaOS::dictionary* DtaWindows::getOSSpecificInformation(OSDEVICEHANDLE osDeviceHandle,
                                                 const char * devref,
                                                 InterfaceDeviceID& interfaceDeviceIdentification,
                                                 DTA_DEVICE_INFO& device_info)
{
    dictionary* presult = new dictionary;
    dictionary& property = *presult;

    DISK_GEOMETRY_EX dg = { 0 };

    /*  determine the attachment type of the drive */
    BYTE descriptorStorage[4096];
    memset(descriptorStorage, 0, sizeof(descriptorStorage));
    STORAGE_DEVICE_DESCRIPTOR& descriptor = *(STORAGE_DEVICE_DESCRIPTOR*)&descriptorStorage;

    STORAGE_PROPERTY_QUERY query;
    memset(&query, 0, sizeof(query));
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    DWORD BytesReturned;

    if (!DeviceIoControl(
        osDeviceHandle,									// handle to a device
        IOCTL_STORAGE_QUERY_PROPERTY,		    // dwIoControlCode
        &query,								// input buffer - STORAGE_PROPERTY_QUERY structure
        sizeof(STORAGE_PROPERTY_QUERY),		// size of input buffer
        descriptorStorage,					// output buffer
        sizeof(descriptorStorage),	    	// size of output buffer
        &BytesReturned,						// number of bytes returned
        NULL)) {
        //LOG(E) << "Can not determine the device type";
        return NULL;
    }

    device_info.devType = DEVICE_TYPE_OTHER;

    // We get some information to fill in to the device information struct as
    // defaults in case other efforts are fruitless

#define copyIfAvailable(descriptorField,deviceInfoField,key)                   \
  do {                                                                        \
       size_t offset=descriptor.descriptorField##Offset;                      \
       if (offset!=0) {                                                       \
          const char * property = (const char *)(&descriptorStorage[offset]); \
          strncpy_s((char *)(&device_info.deviceInfoField),                   \
                     sizeof(device_info.deviceInfoField) +                    \
                        sizeof(device_info.deviceInfoField##Null),            \
	                 property, strlen(property));                             \
          device_info.deviceInfoField##Null='\0';                             \
          (*presult)[key]=property;                                           \
	   }                                                                      \
  } while(0)

    copyIfAvailable(VendorId, vendorID, "vendorID");
    copyIfAvailable(ProductId, modelNum, "modelNum");
    copyIfAvailable(ProductRevision, firmwareRev, "firmwareRev");
    copyIfAvailable(SerialNumber, serialNum, "serialNum");

    {
        unsigned char* p = interfaceDeviceIdentification;
#define copyIDField(field, length) do { memcpy(p, device_info.field, length); p += length; } while (0)
        copyIDField(vendorID, INQUIRY_VENDOR_IDENTIFICATION_Length);
        copyIDField(modelNum, INQUIRY_PRODUCT_IDENTIFICATION_Length);
        copyIDField(firmwareRev, INQUIRY_PRODUCT_REVISION_LEVEL_Length);
    }


    switch (descriptor.BusType) {
    case BusTypeAta:
        LOG(D4) << devref << " descriptor.BusType = BusTypeAta (" << descriptor.BusType << ")";
        property["busType"] = "ATA";
        device_info.devType = DEVICE_TYPE_ATA;
        break;

    case BusTypeSata:
        LOG(D4) << devref << " descriptor.BusType = BusTypeSata (" << descriptor.BusType << ")";
        property["busType"] = "SATA";
        device_info.devType = DEVICE_TYPE_ATA;
        break;

    case BusTypeUsb:
        LOG(D4) << devref << " descriptor.BusType = BusTypeUsb (" << descriptor.BusType << ")";
        property["busType"] = "USB";
        device_info.devType = DEVICE_TYPE_USB;
        break;

    case BusTypeNvme:
        LOG(D4) << devref << " descriptor.BusType = BusTypeNvme (" << descriptor.BusType << ")";
        property["busType"] = "NVME";
        device_info.devType = DEVICE_TYPE_NVME;
        break;

    case BusTypeRAID:
        LOG(D4) << devref << " descriptor.BusType = BusTypeRAID (" << descriptor.BusType << ")";
        property["busType"] = "RAID";
        device_info.devType = DEVICE_TYPE_OTHER;
        break;

    case BusTypeSas:
        LOG(D4) << devref << " descriptor.BusType = BusTypeSas (" << descriptor.BusType << ")";
        property["busType"] = "SAS";
        device_info.devType = DEVICE_TYPE_SAS;
        break;

    default:
        LOG(D4) << devref << " has UNKNOWN descriptor.BusType " << descriptor.BusType << "?!";
        property["busType"] = "UNKN";
        device_info.devType = DEVICE_TYPE_OTHER;
        break;
    }


    // We can fill in the size (capacity) of tosDeviceHandlee device regardless of its type
    //
    if (DeviceIoControl(osDeviceHandle,                 // handle to device
        IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, // dwIoControlCode
        NULL,                             // lpInBuffer
        0,                                // nInBufferSize
        &dg,             // output buffer
        sizeof(dg),           // size of output buffer
        &BytesReturned,        // number of bytes returned
        NULL)) {
        device_info.devSize = dg.DiskSize.QuadPart;
        LOG(D4) << devref << " size = " << device_info.devSize;
    }
    else {
        device_info.devSize = 0;
        LOG(D4) << devref << " size is UNKNOWN";
    }
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%llu", device_info.devSize);
    property["size"] = std::string(buffer);

    return presult;
}

void * DtaWindows::alloc_aligned_MIN_BUFFER_LENGTH_buffer () {
    return _aligned_malloc( IO_BUFFER_ALIGNMENT,
                            (static_cast<size_t>(((MIN_BUFFER_LENGTH + IO_BUFFER_ALIGNMENT - 1) / IO_BUFFER_ALIGNMENT))
                              * IO_BUFFER_ALIGNMENT) );
}

void DtaWindows::free_aligned_MIN_BUFFER_LENGTH_buffer (void * aligned_buffer) {
    _aligned_free(aligned_buffer);
}



/** Perform a SCSI command using the current operating system SCSI interface
 *
 * @param osDeviceHandle            OSDEVICEHANDLE osDeviceHandle of already-opened raw device file
 * @param dxfer_direction direction of transfer PSC_FROM/TO_DEV
 * @param cdb             SCSI command data buffer
 * @param cdb_len         length of SCSI command data buffer (often 12)
 * @param buffer          SCSI data buffer
 * @param bufferlen       SCSI data buffer len, also output transfer length
 * @param sense           SCSI sense data buffer
 * @param senselen        SCSI sense data buffer len (usually 32?)
 * @param pmasked_status  pointer to storage for masked_status, or NULL if not desired
 * @param timeout         optional timeout (in msecs)
 *
 * Returns the result of the os system call, as well as possibly setting *pmasked_status
 */
int DtaWindows::PerformSCSICommand(OSDEVICEHANDLE osDeviceHandle,
                                 int dxfer_direction,
                                 uint8_t * cdb,   unsigned char cdb_len,
                                 void * buffer,   unsigned int& bufferlen,
    unsigned char* sense, unsigned char & senselen,
    SCSI_STATUS_CODE * pmasked_status,
                                 unsigned int timeout)
{
    if (osDeviceHandle <= 0) {
        LOG(E) << "Scsi device not open";
        return EBADF;
    }
    /*
   * Initialize the SCSI_PASS_THROUGH_DIRECT structures
   * per windows DOC with the SCSI Command set reference
   */
   /** Device specific implementation of disk access functions. */
    typedef struct _SDWB {
        SCSI_PASS_THROUGH_DIRECT sd;
        WORD filler;
        unsigned char sensebytes[32];
    } SDWB;

    SDWB* scsi = (SDWB*)alloc_aligned_MIN_BUFFER_LENGTH_buffer();
    memset(scsi, 0, sizeof(SDWB));
    scsi->sd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsi->sd.ScsiStatus = 0;
    scsi->sd.PathId = 0;
    scsi->sd.TargetId = 0;
    scsi->sd.Lun = 0;
    scsi->sd.SenseInfoOffset = offsetof(SDWB, sensebytes);
    scsi->sd.DataIn = ((dxfer_direction == PSC_FROM_DEV) ? SCSI_IOCTL_DATA_IN : SCSI_IOCTL_DATA_OUT);
    scsi->sd.DataBuffer = buffer;
    scsi->sd.DataTransferLength = bufferlen;
    scsi->sd.TimeOutValue = 20;
    scsi->sd.SenseInfoLength = sizeof(scsi->sensebytes);
    scsi->sd.CdbLength = cdb_len;
    memcpy_s(scsi->sd.Cdb, sizeof(scsi->sd.Cdb), cdb, cdb_len);

    DWORD bytesReturned = -1;
    BOOL result = DeviceIoControl(osDeviceHandle, // device to be queried
        IOCTL_SCSI_PASS_THROUGH_DIRECT, // operation to perform
        scsi, sizeof(SDWB),
        scsi, sizeof(SDWB),
        &bytesReturned, // # bytes returned
        (LPOVERLAPPED)NULL);

    if (pmasked_status != NULL) *pmasked_status = static_cast<SCSI_STATUS_CODE>(scsi->sd.ScsiStatus);
    if (sense != NULL) memcpy_s(sense, senselen, scsi->sensebytes, sizeof(scsi->sensebytes));
    if (result) {
        bufferlen = bytesReturned;
    }

    free_aligned_MIN_BUFFER_LENGTH_buffer((void*)scsi);
    return result ? 0 : -1;

}



/** Perform a NVMe command using the `nvme_admin_cmd' (NVMe standard) interface with ioctl `NVME_IOCTL_ADMIN_CMD'
 *
 * @param osDeviceHandle  OSDEVICEHANDLE of already-opened raw device file
 * @param pcmd             NVMe command struct
 *
 * Returns the result of the os system call, as well as possibly setting *pstatus
 */
int DtaWindows::PerformNVMeCommand(OSDEVICEHANDLE osDeviceHandle,
                                   uint8_t * pcmd,
                                   uint32_t * /*pstatus*/)
{
  if (osDeviceHandle==INVALID_HANDLE_VALUE) {
    LOG(E) << "Nvme device not open";
    return EBADF;
  }

  if (pcmd == NULL)
    return DTAERROR_COMMAND_ERROR;
  nvme_admin_cmd & cmd=*(reinterpret_cast<nvme_admin_cmd *>(pcmd));

  /*
   * Do the IO
   */

  IFLOG(D4) {
    LOG(D4) << "DtaWindows::PerformNVMeCommand cmd:" ;
    DtaHexDump(&cmd, sizeof(cmd));
  }

  LOG(E) << "DtaWindows::PerformNVMeCommand NOT IMPLEMENTED!!";

  return DTAERROR_FAILURE;

}
