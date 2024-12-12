/* C:B**************************************************************************
   This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

#include <algorithm>

#include <linux/fs.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <systemd/sd-device.h>

#include <scsi/sg.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
#include <linux/nvme_ioctl.h>
#else
#include <linux/nvme.h>
#endif
#include <nvme/types.h>

#include "DtaLinux.h"

#include "DtaHexDump.h"





DtaOS * DtaOS::getDtaOS () { return new DtaLinux(); }

OSDEVICEHANDLE DtaLinux::openDeviceHandle(const char * devref, bool & accessDenied){
  LOG(D4) << "openDeviceHandle(\"" << devref << "\", _)";
  int descriptor=open(devref, O_RDWR);
  LOG(D4) << "openDeviceHandle(\"" << devref << "\", _)"
          << "=> descriptor=" << descriptor;
  if (descriptor == -1) {
    switch (errno) {
    case EACCES:
      accessDenied = true;
      LOG(D4) << "openDeviceHandle(\"" << devref << "\", _)"
              << " access denied";
      break;
    case ENOENT:
      LOG(E) << "No such device: " << devref;
      break;
    default:
      LOG(E) << "Failed opening " << devref << " with error " << errno << ": " << strerror(errno) ;
      break;
    }
    return INVALID_HANDLE_VALUE;
  }

  return handle(descriptor);

}

void DtaLinux::closeDeviceHandle(OSDEVICEHANDLE osDeviceHandle){
  LOG(D4) << "Entering DtaLinux::closeDeviceHandle";
  int descriptor = handleDescriptor(osDeviceHandle);
  LOG(D4) << "DtaLinux::closeDeviceHandle -- calling close(" << descriptor << ")...";
  close(descriptor);
  LOG(D4) << "DtaLinux::closeDeviceHandle -- returned from close(" << descriptor << ")";
  LOG(D4) << "Exiting DtaLinux::closeDeviceHandle";
}


std::vector<std::string> DtaLinux::generateDtaDriveDevRefs()
{
  std::vector<std::string> devrefs;

  DIR *dir = opendir("/dev");
  if (dir==NULL) {
    LOG(E) << "Can't read /dev ?!";
    return devrefs;
  }

  struct dirent *dirent;
  while (NULL != (dirent=readdir(dir))) {
    std::string devref=std::string("/dev/")+dirent->d_name;

    struct stat s;
    stat(devref.c_str(), &s);
    // LOG(E) << devref
    //        << " st_ino:" << HEXON(4) << s.st_ino
    //        << " st_dev:" << HEXON(4) << s.st_dev
    //        << " st_rdev:" << HEXON(4) << s.st_rdev
    //        << " st_rdev>>8:" << HEXON(4) << (s.st_rdev>>8)
    //        << " st_rdev&0xF:" << HEXON(4) << (s.st_rdev & 0x000F)
    //   ;
    const unsigned long device_type=s.st_rdev >> 8;
    const unsigned char device_part=s.st_rdev & 15;
    typedef enum _rdev_type {
      SCSI_DRIVE=8,
      NVME_DRIVE=259,
    } rdev_type;
    if (device_part==0 &&
        (device_type==rdev_type::SCSI_DRIVE ||
         device_type==rdev_type::NVME_DRIVE)) {
      // LOG(E) << devref << " accepted."
      //   ;
      devrefs.push_back(devref);
    }

  }

  closedir(dir);

  std::sort(devrefs.begin(),devrefs.end());

  return devrefs;
}


void DtaLinux::errorNoAccess(const char* devref) {
  if (devref == NULL) {
    LOG(E) << "You do not have permission to access the raw disk(s) in write mode";
  } else {
    LOG(E) << "You do not have permission to access the raw disk " << devref << " in write mode";
  }
  LOG(E) << "Perhaps you might try sudo to run as root";
}



DtaOS::dictionary * DtaLinux::getOSSpecificInformation(OSDEVICEHANDLE osDeviceHandle,
                                                       const char * devref,
                                                       InterfaceDeviceID & interfaceDeviceIdentification,
                                                       DTA_DEVICE_INFO &device_info) {

  device_info.devType = DEVICE_TYPE_OTHER;

  int r;

  // Special `ioctl` to get the device size
  device_info.devSize = 0;
  r = ioctl(handleDescriptor(osDeviceHandle), BLKGETSIZE64, &device_info.devSize);
  if (r < 0) {
    errno = -r;
    fprintf(stderr, "Failed to get device size: %m for device %s osDeviceHandle 0x%16p\n", devref, osDeviceHandle);
  }

  // Get the `sd_device` to extract properties
  __attribute__((cleanup(sd_device_unrefp))) sd_device *device = NULL;
  r = sd_device_new_from_devname(&device, devref);
  if (r < 0) {
    errno = -r;
    fprintf(stderr, "Failed to allocate sd_device: %m\n");
    return NULL;
  }

  // Get device properties from `sd_device device`
  dictionary * pDeviceProperties = new dictionary;
  dictionary & deviceProperties = *pDeviceProperties;
  // const char *value, *key;
  // FOREACH_DEVICE_PROPERTY(device, key, value) deviceProperties[key] = value ;
  const char *key, *value;
  for (key = sd_device_get_property_first(device, &value);
       key != NULL;
       key = sd_device_get_property_next(device, &value))
    {
      deviceProperties[key] = value ;
    }

  // Done with `sd_device device`
  sd_device_unref(device);


  // Copy device properties from `deviceProperties` into `device_info`
#define getDeviceProperty(key,field)                                    \
  do                                                                    \
    if (1==deviceProperties.count(#key)) {                              \
      std::string deviceProperty(deviceProperties[#key]);               \
      LOG(D3) << #key << " is " << deviceProperty;                      \
      safecopy(device_info.field, sizeof(device_info.field), (uint8_t *)deviceProperty.c_str(), strlen(deviceProperty.c_str())); \
    } while (0)

  LOG(D3) << "Device properties from os:";
  getDeviceProperty(ID_SERIAL_SHORT,serialNum) ;
  getDeviceProperty(ID_MODEL,modelNum) ;
  getDeviceProperty(ID_REVISION,firmwareRev) ;
  getDeviceProperty(ID_VENDOR,vendorID) ;


  if (1==deviceProperties.count("ID_WWN")) {
    std::string str_WWN(deviceProperties["ID_WWN"]);
    LOG(D3) << "ID_WWN is " << str_WWN;
    std::transform(str_WWN.begin(), str_WWN.end(), str_WWN.begin(), ::toupper);
    LOG(D3) << "ID_WWN in uppercase is " << str_WWN;
    // Various WWN hacks
    // Might start with "0X"
    if (str_WWN.substr(0,2)=="0X") {
        str_WWN=str_WWN.substr(2);
    // Might start with "EUI."
    } else if (str_WWN.substr(0,4)=="EUI.") {
        str_WWN=str_WWN.substr(4);
    }
    size_t WWN_length=str_WWN.length();
    size_t device_info_nybbles = 2 * sizeof(device_info.worldWideName);

    intptr_t length_difference =  device_info_nybbles - 1 - WWN_length ;


    if (0 < length_difference) {
      str_WWN += std::string(length_difference, '0');
    } else {
      str_WWN = str_WWN.substr(length_difference, device_info_nybbles - 1);
    }
    str_WWN = std::string(1, '5') + str_WWN;

    LOG(D3) << "str_WWN is " << str_WWN;
    LOG(D3) << "str_WWN.length()=" << str_WWN.length();
    LOG(D3) << "sizeof(device_info.worldWideName)=" << sizeof(device_info.worldWideName);
    assert(str_WWN.length()==device_info_nybbles);

    unsigned char *dst=device_info.worldWideName;
    const unsigned char *str_src=reinterpret_cast<const unsigned char *>(str_WWN.c_str());
    const unsigned char *dst_end=dst+sizeof(device_info.worldWideName);
    #define nybble_value(c)                                                 \
    static_cast<uint8_t>(('0'<=c && c<='9') ? (c-'0')    :                  \
                         ('A'<=c && c<='F') ? (c-'A'+10) :                  \
                         (assert(('0'<=c && c<='9') || ('A'<=c && c<='F')), \
                          0))

    while(dst<dst_end) {
      unsigned char hi=*str_src++;
      unsigned char lo=*str_src++;
      *dst++=nybble_value(hi)<<4 | nybble_value(lo);
    }
  }


  // Special brute-force copy into `device_info.passwordSalt`
  memcpy(device_info.passwordSalt, device_info.serialNum, sizeof(device_info.passwordSalt));


  // Copy `device_info` fields into `interfaceDeviceIndentification` blob for special cases
  uint8_t * p = (uint8_t *)interfaceDeviceIdentification;
#define copyDeviceIdentificationField(field,size)                       \
  do { memcpy(p,device_info.field, size); p += size; } while (0)

  copyDeviceIdentificationField(vendorID,INQUIRY_VENDOR_IDENTIFICATION_Length);
  copyDeviceIdentificationField(modelNum,INQUIRY_PRODUCT_IDENTIFICATION_Length);
  copyDeviceIdentificationField(firmwareRev,INQUIRY_PRODUCT_REVISION_LEVEL_Length);

  std::string bus=deviceProperties["ID_BUS"];
  std::string devpath=deviceProperties["DEVPATH"];


  if (bus=="scsi") {
    device_info.devType = DEVICE_TYPE_SCSI;
  } else if (bus == "usb") {
    if (deviceProperties["ID_USB_DRIVER"]=="uas") {
      device_info.devType = DEVICE_TYPE_SAS;
      //    } else if (deviceProperties["ID_USB_DRIVER"]=="usb-storage") {
      //      device_info.devType = DEVICE_TYPE_NVME;
    }
  } else if (bus == "ata") {
    if (deviceProperties["ID_USB_DRIVER"]=="uas") {
      device_info.devType = DEVICE_TYPE_USB;
    } else {
      device_info.devType = DEVICE_TYPE_ATA;
    }
  } else if (bus == "nvme"
          || devpath.find("/nvme/") != std::string::npos) {
    device_info.devType = DEVICE_TYPE_NVME;
  }

  // Return properties dictionary both as in indication of success and for futher mischief
  return pDeviceProperties;
}



void * DtaLinux::alloc_aligned_MIN_BUFFER_LENGTH_buffer () {
  return aligned_alloc( IO_BUFFER_ALIGNMENT,
                        (((MIN_BUFFER_LENGTH + IO_BUFFER_ALIGNMENT - 1)
                          / IO_BUFFER_ALIGNMENT)
                         * IO_BUFFER_ALIGNMENT) );
}

void DtaLinux::free_aligned_MIN_BUFFER_LENGTH_buffer (void * aligned_buffer) {
  free(aligned_buffer);
}

/** Perform a SCSI command using the Linux `sg' (SCSI generic) interface with ioctl `SG_IO'
 *
 * @param osDeviceHandle            OSDEVICEHANDLE osDeviceHandle of already-opened raw device file
 * @param dxfer_direction direction of transfer PSC_FROM/TO_DEV
 * @param cdb             address of SCSI command data buffer
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
int DtaLinux::PerformSCSICommand(OSDEVICEHANDLE osDeviceHandle,
                                 int dxfer_direction,
                                 uint8_t * cdb,   unsigned char cdb_len,
                                 void * buffer,   unsigned int& bufferlen,
                                 unsigned char * sense, unsigned char & senselen,
                                 SCSI_STATUS_CODE * pmasked_status,
                                 unsigned int timeout)
{
  if (osDeviceHandle==INVALID_HANDLE_VALUE) {
    LOG(E) << "Scsi device not open";
    return EBADF;
  }

  if (cdb == NULL)
    return DTAERROR_COMMAND_ERROR;

  sg_io_hdr_t sg;
  memset(&sg, 0, sizeof(sg));

  sg.interface_id = 'S';
  sg.dxfer_direction = dxfer_direction;  // We pun on dxfer_direction so no conversion
  sg.cmd_len = cdb_len;
  sg.mx_sb_len = senselen;
  sg.dxfer_len = bufferlen;
  sg.dxferp = buffer;
  sg.cmdp = cdb;
  sg.sbp = sense;
  sg.timeout = timeout;

  IFLOG(D4)
    if (dxfer_direction ==  PSC_TO_DEV) {
      LOG(D4) << " DtaLinux::PerformSCSICommand buffer before";
      DtaHexDump(buffer,bufferlen);
    }


  /*
   * Do the IO
   */

  IFLOG(D4) {
    LOG(D4) << " DtaLinux::PerformSCSICommand sg:" ;
    DtaHexDump(&sg, sizeof(sg));
    LOG(D4) << "DtaLinux::PerformSCSICommand cdb before:" ;
    DtaHexDump(cdb, cdb_len);
  }
  LOG(D4) << " DtaLinux::PerformSCSICommand calling ioctl ...";
  int kernResult = ioctl(handleDescriptor(osDeviceHandle), SG_IO, &sg);
  if (0 != kernResult) {
    LOG(E) << "DtaLinux::PerformSCSICommand:: ioctl(" << handleDescriptor(osDeviceHandle)
           << ", ...) failed with kernResult " << HEXON(8) << kernResult;
    LOG(E) << "DtaLinux::PerformSCSICommand:: sg.masked_status=" << (int)sg.masked_status;
  }
  LOG(D4) << " DtaLinux::PerformSCSICommand ioctl kernResult=" << HEXON(8) << kernResult ;
  IFLOG(D4) {
    if (0 != kernResult) {
      LOG(D4) << " DtaLinux::PerformSCSICommand cdb after ioctl returned "
              << HEXON(8) << kernResult << " (" << strerror(kernResult) << ")" ;
      DtaHexDump(cdb, cdb_len);
      if (sg.masked_status != GOOD) {
        LOG(D4)
          << "DtaLinux::PerformSCSICommand cdb after with masked_status == " << statusName(sg.masked_status)
          << " == " << std::hex << (int)sg.masked_status;
      }
      if (sense != NULL) {
        LOG(D4) << "DtaLinux::PerformSCSICommand sense after ";
        DtaHexDump(sense, senselen);
      }
    }
  }

  // Without any real justification we set bufferlen to the value of dxfer_len - resid
  bufferlen = sg.dxfer_len - sg.resid;

  senselen = sg.sb_len_wr;

  if (pmasked_status != NULL) {
    *pmasked_status = static_cast<SCSI_STATUS_CODE>(sg.masked_status);
  }

  IFLOG(D4)
    if (dxfer_direction == PSC_FROM_DEV && 0 == kernResult && sg.masked_status == GOOD) {
      LOG(D4) << "DtaLinux::PerformSCSICommand buffer after kIOReturnSuccess == kernResult && sg.masked_status == GOOD:";
      DtaHexDump(buffer, bufferlen);
    }



  LOG(D4) << "PerformSCSICommand kernResult=" << HEXON(8) << kernResult ;
  IFLOG(D4) {
    if (kernResult < 0) {
      LOG(D4) << "cdb after returned " << HEXON(8) << kernResult << ":";
      DtaHexDump(cdb, cdb_len);
    } else {
      LOG(D4) << "DtaLinux::PerformSCSICommand buffer after kernResult " << HEXON(8) << kernResult ;
      DtaHexDump(buffer, bufferlen);
    }
  }

  return kernResult ;
}


/** Perform a NVMe command using the Linux `nvme_admin_cmd' (NVMe standard) interface with ioctl `NVME_IOCTL_ADMIN_CMD'
 *
 * @param osDeviceHandle  OSDEVICEHANDLE of already-opened raw device file
 * @param cmd             NVMe command struct
 *
 * Returns the result of the os system call, as well as possibly setting *pmasked_status
 */
int DtaLinux::PerformNVMeCommand(OSDEVICEHANDLE osDeviceHandle,
                                 uint8_t * pcmd)
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
    LOG(D4) << " DtaLinux::PerformNVMeCommand cmd:" ;
    DtaHexDump(&cmd, sizeof(cmd));
  }
  LOG(D4) << " DtaLinux::PerformNVMeCommand calling ioctl ...";
  int kernResult =  ioctl(handleDescriptor(osDeviceHandle), NVME_IOCTL_ADMIN_CMD, &cmd);

  if (0 != kernResult) {
    LOG(E) << "DtaLinux::PerformNVMeCommand:: ioctl(" << handleDescriptor(osDeviceHandle)
           << ", ...) failed with kernResult " << HEXON(8) << kernResult;
  }
  LOG(D4) << " DtaLinux::PerformNVMeCommand ioctl kernResult=" << HEXON(8) << kernResult ;
  IFLOG(D4) {
    if (0 != kernResult) {
      LOG(D4) << " DtaLinux::PerformNVMeCommand cmd after ioctl returned "
              << HEXON(8) << kernResult << " (" << strerror(kernResult) << ")" ;
      DtaHexDump(&cmd, sizeof(cmd));
    }
  }


  return kernResult ;
}
