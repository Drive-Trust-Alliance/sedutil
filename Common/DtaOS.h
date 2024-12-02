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
#pragma once

#include <map>
#include <string>
#include <vector>
#include "os.h"
#include "ATAStructures.h"
#include "DtaStructures.h"
#include "DtaHexDump.h"


/*
 *  SCSI Status codes
 */
typedef enum _SCSI_STATUS_CODE {
  GOOD                 = 0x00 ,
  CHECK_CONDITION      = 0x01 ,
  CONDITION_GOOD       = 0x02 ,
  BUSY                 = 0x04 ,
  INTERMEDIATE_GOOD    = 0x08 ,
  INTERMEDIATE_C_GOOD  = 0x0a ,
  RESERVATION_CONFLICT = 0x0c ,
  COMMAND_TERMINATED   = 0x11 ,
  QUEUE_FULL           = 0x14 ,
  STATUS_MASK          = ( GOOD
                         | CHECK_CONDITION
                         | CONDITION_GOOD
                         | BUSY
                         | INTERMEDIATE_GOOD
                         | INTERMEDIATE_C_GOOD
                         | RESERVATION_CONFLICT
                         | COMMAND_TERMINATED
                         | QUEUE_FULL
                         )
} SCSI_STATUS_CODE;
#define CaseForStatus(status) case status: return std::string( #status )
template <typename status_type> static inline std::string statusName(status_type statusValue)
{
  switch (static_cast<SCSI_STATUS_CODE>(statusValue))
    {
      CaseForStatus( GOOD                 );
      CaseForStatus( CHECK_CONDITION      );
      CaseForStatus( CONDITION_GOOD       );
      CaseForStatus( BUSY                 );
      CaseForStatus( INTERMEDIATE_GOOD    );
      CaseForStatus( INTERMEDIATE_C_GOOD  );
      CaseForStatus( RESERVATION_CONFLICT );
      CaseForStatus( COMMAND_TERMINATED   );
      CaseForStatus( QUEUE_FULL           );
    default: return std::string("????");
    }
}
#undef CaseForStatus





/** Base class for a operating system interface.
 * This is a virtual base class defining the minimum functionality of operating system interface
 * object.  A singleton instance of some concrete subclass must implement the interface to
 * the actual running operating system.
 */




class DtaOS {
protected:
  /** Default constructor, does nothing */
  DtaOS(){};
  /** Default destructor, does nothing*/
  virtual ~DtaOS(){};

  /** Allocate an instance of this subclass for the current operating system **/
  static DtaOS * getDtaOS();

public:

  // DtaOS is a singleton interface to the current operating system as needed by Dta

  DtaOS(DtaOS& other)=delete;
  void operator=(const DtaOS &) = delete;
  static DtaOS& theDtaOSInstance() {
    static DtaOS *
#if defined(__clang__)
      __attribute__((no_destroy))
#endif
      pos;
    if (pos == NULL)
      pos = getDtaOS();
    assert(pos != NULL);
    return *pos;
  }

  // Drive handles and refs (strings in OS lexicon)

  virtual OSDEVICEHANDLE openDeviceHandle(const char * devref, bool & accessDenied) = 0;

  virtual void closeDeviceHandle(OSDEVICEHANDLE osDeviceHandle) = 0;

  virtual std::vector<std::string> generateDtaDriveDevRefs() = 0;

  /** Say the right thing about having access rights: sudo, run as administrator, whatever  **/
  virtual void errorNoAccess(const char * devref) = 0;

  typedef std::map<std::string, std::string>dictionary;

  virtual
  dictionary*
  getOSSpecificInformation(OSDEVICEHANDLE osDeviceHandle,
                           const char* devref,
                           InterfaceDeviceID& interfaceDeviceIdentification,
                           DTA_DEVICE_INFO& device_info) = 0;

  virtual void * alloc_aligned_MIN_BUFFER_LENGTH_buffer () = 0;

  virtual void free_aligned_MIN_BUFFER_LENGTH_buffer (void * aligned_buffer)  = 0;

  bool testDeviceHandle(const char *devref, bool & accessDenied) {
    OSDEVICEHANDLE osDeviceHandle=openDeviceHandle(devref, accessDenied);
    bool result=(osDeviceHandle != INVALID_HANDLE_VALUE);
    if (result) closeDeviceHandle(osDeviceHandle);
    return result;
  }

  virtual
  std::vector<std::string> enumerateDtaDriveDevRefs(bool & accessDenied){
    std::vector<std::string> devrefs;
    for (std::string & str_devref:generateDtaDriveDevRefs()) {
      const char * devref=str_devref.c_str();
      bool accessDeniedThisTime=false;
      if (testDeviceHandle(devref, accessDeniedThisTime))
        devrefs.push_back(str_devref);
      else if (accessDeniedThisTime && !accessDenied) {
        //            errorNoAccess(devref);
        accessDenied=true;
      }
    }
    return devrefs;
  }


  OSDEVICEHANDLE openAndCheckDeviceHandle(const char * devref, bool& accessDenied) {
    OSDEVICEHANDLE osDeviceHandle = openDeviceHandle(devref, accessDenied);
    if (INVALID_HANDLE_VALUE == osDeviceHandle || accessDenied) {
      LOG(D1) << "Error opening device " << devref << " -- not found";
    }
    return osDeviceHandle;
  }



    // Stolen from sg.h in case we are not Linux ...
  #if !defined(SG_DXFER_TO_DEV)
  #define SG_DXFER_TO_DEV -2      /* e.g. a SCSI WRITE command */
  #endif  // !defined(SG_DXFER_TO_DEV)
  #if !defined(SG_DXFER_FROM_DEV)
  #define SG_DXFER_FROM_DEV -3    /* e.g. a SCSI READ command */
  #endif  // !defined(SG_DXFER_FROM_DEV)

  #define PSC_TO_DEV   SG_DXFER_TO_DEV
  #define PSC_FROM_DEV SG_DXFER_FROM_DEV


    /** Perform an ATA command using the current operating system HD interface
     *
     * @param osDeviceHandle    OSDEVICEHANDLE of already-opened raw device file
     * @param cmd               ATACOMMAND opcode IDENTIFY_DEVICE, TRUSTED_SEND, or TRUSTED_RECEIVE
     * @param securityProtocol  security protocol ID per ATA command spec
     * @param comID             communication channel ID per TCG spec
     * @param buffer            address of data buffer
     * @param bufferlen         data buffer len, also output transfer length
     *
     * Returns the result of the os system call
     */
    virtual int PerformATACommand_via_HD(OSDEVICEHANDLE osDeviceHandle,
                                  ATACOMMAND cmd, uint8_t securityProtocol, uint16_t comID,
                                  void * buffer,  unsigned int & bufferlen) = 0;


    /** Perform an ATA command using SCSI/ATA translation
     *
     * @param osDeviceHandle    OSDEVICEHANDLE of already-opened raw device file
     * @param cmd               ATACOMMAND opcode IDENTIFY_DEVICE, TRUSTED_SEND, or TRUSTED_RECEIVE
     * @param securityProtocol  security protocol ID per ATA command spec
     * @param comID             communication channel ID per TCG spec
     * @param buffer            address of data buffer
     * @param bufferlen         data buffer len, also output transfer length
     *
     * Returns the result of the os system call
     */
    virtual int PerformATACommand_via_SAT(OSDEVICEHANDLE osDeviceHandle,
                                  ATACOMMAND cmd, uint8_t securityProtocol, uint16_t comID,
                                  void * buffer,  unsigned int & bufferlen)
    {
      uint8_t protocol;
      int dxfer_direction;
      unsigned int timeout;

      switch (cmd)
        {
        case IDENTIFY_DEVICE:
          timeout=600;  //  IDENTIFY sg.timeout = 600; // Sabrent USB-SATA adapter 1ms,6ms,20ms,60 NG, 600ms OK
          protocol = PIO_DATA_IN;
          dxfer_direction = PSC_FROM_DEV;
          break;

        case TRUSTED_RECEIVE:
          timeout=60000;
          protocol = PIO_DATA_IN;
          dxfer_direction = PSC_FROM_DEV;
          break;

        case TRUSTED_SEND:
          timeout=60000;
          protocol = PIO_DATA_OUT;
          dxfer_direction = PSC_TO_DEV;
          break;

        default:
          LOG(E) << "Exiting PerformATACommand_via_SAT because of unrecognized cmd=" << cmd << "?!" ;
          return 0xff;
        }


      CScsiCmdATAPassThrough_12 cdb;
      uint8_t * cdbBytes=(uint8_t *)&cdb;  // We use direct byte pointer because bitfields are unreliable
      cdbBytes[1] = static_cast<uint8_t>(protocol << 1);
      cdbBytes[2] = static_cast<uint8_t>((protocol==PIO_DATA_IN ? 1 : 0) << 3 |  // TDir
                                         1                               << 2 |  // ByteBlock
                                         2                                       // TLength  10b => transfer length in Count
                                         );
      cdb.m_Features = static_cast<uint8_t>(securityProtocol);
      cdb.m_Count = static_cast<uint8_t>(bufferlen/512);
      cdb.m_LBA_Mid = comID & 0xFF;          // ATA lbaMid   / TRUSTED COMID low
      cdb.m_LBA_High = (comID >> 8) & 0xFF;  // ATA lbaHigh  / TRUSTED COMID high
      cdb.m_Command = static_cast<uint8_t>(cmd);

      unsigned char sense[32];
      unsigned char senselen=sizeof(sense);
      memset(&sense, 0, senselen);

      unsigned int dataLength = bufferlen;
      SCSI_STATUS_CODE masked_status=GOOD;

      int result=PerformSCSICommand(osDeviceHandle,
                                    dxfer_direction,
                                    cdbBytes, (unsigned char)sizeof(cdb),
                                    buffer, dataLength,
                                    sense, senselen,
                                    &masked_status,
                                    timeout);
      if (result!=0) {
        LOG(D4) << "PerformATACommand_via_SAT: PerformSCSICommand returned " << result;
        LOG(D4) << "sense after ";
        IFLOG(D4) DtaHexDump(&sense, senselen);
        return 0xff;
      }

      LOG(D4) << "PerformATACommand_via_SAT: PerformSCSICommand returned " << result;
      LOG(D4) << "sense after ";
      IFLOG(D4) DtaHexDump(&sense, senselen);

      // check for successful target completion
      if (masked_status != GOOD)
        {
          LOG(D4) << "PerformATACommand_via_SAT: masked_status=" << masked_status << "=" << statusName(masked_status) << " != GOOD  cmd=" <<
            (cmd == TRUSTED_SEND    ? std::string("TRUSTED_SEND") :
             cmd == TRUSTED_RECEIVE ? std::string("TRUSTED_RECEIVE") :
             cmd == IDENTIFY_DEVICE ? std::string("IDENTIFY_DEVICE") :
             std::to_string(cmd));
          LOG(D4) << "sense after ";
          IFLOG(D4) DtaHexDump(&sense, senselen);
          return 0xff;
        }

      if (! ((0x00 == sense[0]) && (0x00 == sense[1])) ||
          ((0x72 == sense[0]) && (0x0b == sense[1])) ) {
        LOG(D4) << "PerformATACommand_via_SAT: PerformATACommand disqualifying ATA response --"
                << " sense[0]=" << HEXON(2) << (unsigned int)sense[0]
                << " sense[1]=" << HEXON(2) << (unsigned int)sense[1];
        return 0xff; // not ATA response
      }

      LOG(D4) << "buffer after ";
      IFLOG(D4) DtaHexDump(buffer, dataLength);
      LOG(D4) << "PerformATACommand_via_SAT: PerformATACommand returning sense[11]=" << HEXON(2) << (unsigned int)sense[11];
      return (sense[11]);

    }
;


    /*** Perform an ATA command using the current operating system interface
     *
     * @param osDeviceHandle    OSDEVICEHANDLE of already-opened raw device file
     * @param scsiTranslated               bool using SCSI/ATA translation?
     * @param cmd               ATACOMMAND opcode IDENTIFY_DEVICE, TRUSTED_SEND, or TRUSTED_RECEIVE
     * @param securityProtocol  security protocol ID per ATA command spec
     * @param comID             communication channel ID per TCG spec
     * @param buffer            address of data buffer
     * @param bufferlen         data buffer len, also output transfer length
     *
     * Returns the result of the os system call
     */
    virtual int PerformATACommand(OSDEVICEHANDLE osDeviceHandle, bool scsiTranslated,
                                  ATACOMMAND cmd, uint8_t securityProtocol, uint16_t comID,
                                  void * buffer,  unsigned int & bufferlen) {
        return scsiTranslated
          ? PerformATACommand_via_SAT(osDeviceHandle, cmd, securityProtocol, comID, buffer, bufferlen)
          : PerformATACommand_via_HD(osDeviceHandle, cmd, securityProtocol, comID, buffer, bufferlen)
        ;
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
  virtual int PerformSCSICommand(OSDEVICEHANDLE osDeviceHandle,
                                 int dxfer_direction,
                                 uint8_t * cdb,   unsigned char cdb_len,
                                 void * buffer,   unsigned int& bufferlen,
                                 unsigned char * sense, unsigned char & senselen,
                                 SCSI_STATUS_CODE * pmasked_status,
                                 unsigned int timeout=60000) = 0;


  /** Perform a NVMe command using the  `nvme_admin_cmd' (NVMe standard) interface
   *
   * @param osDeviceHandle            OSDEVICEHANDLE osDeviceHandle of already-opened raw device file
   * @param cmd             NVMe command struct
   *
   * Returns the result of the os system call, as well as possibly setting *pmasked_status
   */
  virtual int PerformNVMeCommand(OSDEVICEHANDLE osDeviceHandle,
                                 uint8_t * cmd) = 0;


};

#define OS (DtaOS::theDtaOSInstance())


template <typename T>
static inline bool __is_all_NULs(const T * b, const size_t n) {
  for (const T * e = b + n; b<e; b++) {
    if ( ((T)0)  == *b) continue;
    return false;
  }
  return true;
}


template <typename T>
static inline bool __is_all_zeroes(const T * b, const size_t n) {
  for (const T * e = b + n; b<e; b++) {
    if (((T) 0 ) == *b) continue;
    if (((T)'0') == *b) continue;
    return false;
  }
  return true;
}

#define __is_not_all_NULs(b,n) (!__is_all_NULs(b,n))
#define __is_not_all_zeroes(b,n) (!__is_all_zeroes(b,n))



// If src is all zeros, copying would amount to erasing.  So don't do that.
template <typename T>
static inline void safecopy(T * dst, size_t dstsize, const T * src, size_t srcsize, const T fill = (T)(0))
{
  if (__is_all_NULs(src,srcsize)) return;  // Do not erase dst if src is all zeros

  if (dstsize<=srcsize)
    memcpy(dst,src,dstsize);
  else {
    memcpy(dst,src,srcsize);
    memset(dst+srcsize, fill, dstsize-srcsize);
  }
}


// If src is all zeros, copying would amount to erasing.  So don't do that.
// If dst already contains something other that \s*\0*, don't overwrite it.
template <typename T>
static inline void softcopy(T * dst, size_t dstsize, const T * src, size_t srcsize, const T fill = (T)(0))
{
  if (__is_all_NULs(src,srcsize)) return;  // Do not erase dst if src is all zeros

  T * p = dst, * e = dst+dstsize;
  while (p<e && ((T)' ')==(*p)) p++;     // Skip all initial blanks
  while (p<e && ((T) 0 )==(*p)) p++;     // Skip all subsequent NULs
  if (p!=e) return;                      // Do not replace dst if it was non-empty, i.e. not just maybe some blanks followed by NULs

  if (dstsize<=srcsize)
    memcpy(dst,src,dstsize);
  else {
    memcpy(dst,src,srcsize);
    memset(dst+srcsize, fill, dstsize-srcsize);
  }
}
