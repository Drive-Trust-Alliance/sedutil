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
#include <algorithm>
#include "os.h"
#include "log.h"
#include <dirent.h>

#include "DtaDevOS.h"
#include "DtaHexDump.h"



/** Factory method to produce instance of appropriate subclass
 *   Note that all of DtaDevGeneric, DtaDevEnterprise, DtaDevOpal, ... derive from DtaDevOS
 * @param devref             name of the device in the OS lexicon
 * @param pdev                reference into which to store the address of the new instance
 * @param genericIfNotTPer   if true, store an instance of DtaDevGeneric for non-TPers;
 *                           if false, store NULL for non-TPers
 */
// static
uint8_t DtaDevOS::getDtaDevOS(const char * devref,
                              DtaDevOS * * pdev, bool genericIfNotTPer)
{
    // LOG(D4) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\")";
    DTA_DEVICE_INFO disk_info;
    bzero(&disk_info, sizeof(disk_info));
    
    DtaDevOSDrive * drv = DtaDevOSDrive::getDtaDevOSDrive(devref, disk_info);
    if (drv == NULL) {
        *pdev = NULL;
        // LOG(D4) << "DtaDevOSDrive::getDtaDevOSDrive(\"" << devref <<  "\", disk_info) returned NULL";
        if (!genericIfNotTPer) {  LOG(E) << "Invalid or unsupported device " << devref; }
        // LOG(D4) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_COMMAND_ERROR";
        return DTAERROR_COMMAND_ERROR;
    }
    
    *pdev =  getDtaDevOS(devref, drv, disk_info, genericIfNotTPer) ;
    if (*pdev == NULL) {
        delete drv;
        LOG(D4) << "getDtaDevOS(" << "\"" << devref <<  "\"" << ", "
        << "drive"                 << ", "
        << "disk_info"             << ", "
        << ( genericIfNotTPer ? "true" : "false" )
        <<  ")"
        << " returned NULL";
        if (!genericIfNotTPer) { LOG(E) << "Invalid or unsupported device " << devref; }
        LOG(D4) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_COMMAND_ERROR";
        return DTAERROR_COMMAND_ERROR;
    }
    
    
    LOG(D4) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") disk_info:";
    IFLOG(D4) DtaHexDump(&disk_info, (int)sizeof(disk_info));
    LOG(D4) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_SUCCESS";
    return DTAERROR_SUCCESS;
}


/** The Device class represents a OS generic storage device.
 * At instantiation we determine if we create an instance of the NVMe or SATA or Scsi (SAS) derived class
 */

const unsigned long long DtaDevOS::getSize() { return disk_info.devSize; }

uint8_t DtaDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                          void * buffer, unsigned int bufferlen)
{
    if (!isOpen) return 0xfe; //disk open failed so this will too
    
    if (NULL == drive)
    {
        LOG(E) << "DtaDevOS::sendCmd ERROR - unknown drive type";
        return 0xff;
    }
    
    return drive->sendCmd(cmd, protocol, comID, buffer, bufferlen);
}

bool DtaDevOS::identify(DTA_DEVICE_INFO& disk_info)
{
    return drive->identify(disk_info)
    &&   DTAERROR_SUCCESS == drive->discovery0(disk_info);
}

void DtaDevOS::osmsSleep(uint32_t ms)
{
    usleep(ms * 1000); //convert to microseconds
    return;
}


int  DtaDevOS::diskScan()
{
    LOG(D1) << "Entering DtaDevOS:diskScan ";
    
    IFLOG(D1) {
        fprintf(Output2FILE::Stream(), "Scanning for TCG SWG compliant disks (debug version, loglevel=%d)\n", CLog::Level());
    } else {
        fprintf(Output2FILE::Stream(), "Scanning for Opal compliant disks\n");
    }
    
    vector<string> devRefs=DtaDevOSDrive::enumerateDtaDevOSDriveDevRefs();
    
    IFLOG(D1)
    if (devRefs.size()!=0) {
        fprintf(Output2FILE::Stream(), " device    SSC         Model Number       Firmware  Locn     World Wide Name      Serial Number       Vendor      Manufacturer Name\n");
        fprintf(Output2FILE::Stream(), "---------- ---  ------------ ------------ --------  -----    ----- ---- -----    ------- ---------    -------  --------------- -------\n");
    }
    
    for (string & devref:devRefs) {
        
        DtaDevOS * dev=NULL;
        if (DTAERROR_SUCCESS == getDtaDevOS(devref.c_str(),&dev,true) && dev!=NULL) {
            
            fprintf(Output2FILE::Stream(), "%-11s", devref.c_str());
            if (dev->isAnySSC()) {
                fprintf(Output2FILE::Stream(), " %s%s%s ",
                        (dev->isOpal1()  ? "1" : " "),
                        (dev->isOpal2()  ? "2" : " "),
                        (dev->isEprise() ? "E" : " "));
            } else {
                fprintf(Output2FILE::Stream(), "%s", " No  ");
            }
            
            const char * devType = NULL;
            switch (dev->getDevType()) {
                case DEVICE_TYPE_ATA:
                    devType = "ATA";
                    break;
                case DEVICE_TYPE_SAS:
                    devType = "SAS";
                    break;
                case DEVICE_TYPE_NVME:
                    devType = "NVME";
                    break;
                case DEVICE_TYPE_USB:
                    devType = "USB";
                    break;
                case DEVICE_TYPE_OTHER:
                    devType = "OTHER";
                    break;
                default:
                    devType = "UNKNOWN";
            }
            
            IFLOG(D1) {
                char WWN[19]="                  ";  // 18 blanks as placeholder if missing
                vector<uint8_t>wwn(dev->getWorldWideName());
                if (__is_not_all_NULs(wwn.data(), (unsigned int)wwn.size())) {
                    snprintf(WWN, 19, "%02X%02X%02X%02X%02X%02X%02X%02X %c",
                             wwn[0], wwn[1], wwn[2], wwn[3], wwn[4], wwn[5], wwn[6], wwn[7],
                             dev->isWorldWideNameSynthetic() ? '*' : ' ');
                }
                fprintf(Output2FILE::Stream(), "%-25.25s %-8.8s  %-7.7s  %18s  %-20.20s %-8.8s %-30.30s\n",
                        dev->getModelNum(),
                        dev->getFirmwareRev(),
                        devType,
                        WWN,
                        dev->getSerialNum(),
                        dev->getVendorID(),
                        dev->getManufacturerName());
                
            } else {
                fprintf(Output2FILE::Stream(), "%-25.25s %-8.8s  %-7.7s\n",
                        dev->getModelNum(),
                        dev->getFirmwareRev(),
                        devType);
            }
            
            delete dev;
            dev=NULL;
        }
    }
    
    printf("No more disks present -- ending scan\n");
    LOG(D1) << "Exiting DtaDevOS::scanDisk ";
    return 0;
}

/** Close the device reference so this object can be delete. */
DtaDevOS::~DtaDevOS()
{
    LOG(D4) << "Destroying DtaDevOS";
    if (NULL != drive)
        delete drive;
}
