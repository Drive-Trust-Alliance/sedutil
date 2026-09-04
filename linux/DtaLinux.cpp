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

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <set>

#include <linux/fs.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
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


const std::string DtaOS::name="Linux";
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


/** List the entries of a sysfs directory, without "." and "..".
 *
 * A directory that is not there is not an error: a box with no NVMe driver
 * loaded has no /sys/class/nvme at all.
 */
static std::vector<std::string> sysfsEntries(const std::string & dir)
{
  std::vector<std::string> entries;

  DIR * d = opendir(dir.c_str());
  if (d == NULL) {
    LOG(D2) << "Not scanning " << dir << ": " << strerror(errno);
    return entries;
  }

  struct dirent * e;
  while (NULL != (e = readdir(d)))
    if (e->d_name[0] != '.')   // sysfs has no dotfiles, so this is "." and ".." only
      entries.push_back(e->d_name);
  closedir(d);

  return entries;
}

/** True if `path' is a directory with at least one entry in it. */
static bool sysfsHasEntries(const std::string & path)
{
  DIR * d = opendir(path.c_str());
  if (d == NULL)
    return false;

  bool found = false;
  struct dirent * e;
  while (!found && NULL != (e = readdir(d)))
    found = (e->d_name[0] != '.');
  closedir(d);

  return found;
}

/** Read the first line of a sysfs attribute file, without its newline. */
static bool readSysfsAttribute(const std::string & path, std::string & value)
{
  std::ifstream f(path.c_str());
  if (!f.is_open() || !std::getline(f, value))
    return false;

  while (!value.empty() && (value.back() == '\r' || value.back() == ' '))
    value.pop_back();

  return true;
}

/** Read one KEY=value property out of a sysfs `uevent' file. */
static bool readUeventProperty(const std::string & dir, const char * key, std::string & value)
{
  std::ifstream f((dir + "/uevent").c_str());
  if (!f.is_open())
    return false;

  const std::string prefix = std::string(key) + "=";
  std::string line;
  while (std::getline(f, line))
    if (0 == line.compare(0, prefix.length(), prefix)) {
      value = line.substr(prefix.length());
      while (!value.empty() && value.back() == '\r')
        value.pop_back();
      return true;
    }

  return false;
}

/** Read a sysfs SCSI-ish `device/type' attribute.
 *
 * Absent is a legitimate answer, and a common one: NVMe namespaces, virtio-blk
 * and mmcblk all have a `device' link but no `type' under it.
 */
static bool readSysfsDeviceType(const std::string & devicedir, long & type)
{
  std::string value;
  if (!readSysfsAttribute(devicedir + "/type", value))
    return false;

  char * end = NULL;
  const long parsed = strtol(value.c_str(), &end, 10);
  if (end == value.c_str() || *end != '\0')
    return false;

  type = parsed;
  return true;
}

/** Turn a sysfs node into the /dev path the kernel says it has. */
static std::string devrefForSysfsNode(const std::string & dir, const std::string & sysfsName)
{
  std::string devname;
  if (!readUeventProperty(dir, "DEVNAME", devname) || devname.empty()) {
    // Should not happen for the classes we walk; fall back to the sysfs name,
    // which spells '/' as '!'.
    devname = sysfsName;
    std::replace(devname.begin(), devname.end(), '!', '/');
    LOG(D2) << dir << "/uevent has no DEVNAME, assuming /dev/" << devname;
  }
  return std::string("/dev/") + devname;
}

/** Order device names the way a human reads them.
 *
 * Runs of digits compare as numbers, so nvme2 comes before nvme10.  A run of
 * letters that reaches the end of the name compares by length first, so sdz
 * comes before sdaa -- the kernel hands out sd suffixes as a counter, not as
 * dictionary words.
 */
static bool devrefNaturalLess(const std::string & a, const std::string & b)
{
  size_t i = 0, j = 0;

  while (i < a.length() && j < b.length()) {
    const bool aDigit = (0 != isdigit(static_cast<unsigned char>(a[i])));
    const bool bDigit = (0 != isdigit(static_cast<unsigned char>(b[j])));
    if (aDigit != bDigit)
      return aDigit;

    size_t ei = i;
    while (ei < a.length() && aDigit == (0 != isdigit(static_cast<unsigned char>(a[ei])))) ei++;
    size_t ej = j;
    while (ej < b.length() && bDigit == (0 != isdigit(static_cast<unsigned char>(b[ej])))) ej++;

    if (aDigit) {
      // Compare as numbers: ignore leading zeroes, then more digits means bigger.
      size_t si = i; while (si + 1 < ei && a[si] == '0') si++;
      size_t sj = j; while (sj + 1 < ej && b[sj] == '0') sj++;
      if ((ei - si) != (ej - sj))
        return (ei - si) < (ej - sj);
      const int c = a.compare(si, ei - si, b, sj, ej - sj);
      if (c != 0)
        return c < 0;
    } else {
      if (ei == a.length() && ej == b.length() && (ei - i) != (ej - j))
        return (ei - i) < (ej - j);
      const int c = a.compare(i, ei - i, b, j, ej - j);
      if (c != 0)
        return c < 0;
    }

    i = ei;
    j = ej;
  }

  return (a.length() - i) < (b.length() - j);
}

/** SCSI peripheral device type 0, "direct-access block device" -- a disk.
 *
 * Tape is 1, CD-ROM is 5 and SCSI enclosures are 13; none of those are SEDs.
 */
#define SCSI_TYPE_DIRECT_ACCESS 0

std::vector<std::string> DtaLinux::generateDtaDriveDevRefs()
{
  std::vector<std::string> devrefs;

  // Read /sys/block instead of /dev to find the devices. Relying on
  // names or major/minor numbers in /dev is not reliable.
  DIR * sysblock = opendir("/sys/block");
  if (sysblock == NULL) {
    LOG(E) << "Cannot read /sys/block: " << strerror(errno) << " -- is /sys mounted?";
    LOG(E) << "No drives can be enumerated.";
    return devrefs;
  }
  closedir(sysblock);

  std::set<std::string> seen;
#define addCandidate(devref, why)                               \
  do {                                                          \
    const std::string candidate(devref);                        \
    if (seen.insert(candidate).second) {                        \
      LOG(D2) << "Scanning " << candidate << " -- " << why;     \
      devrefs.push_back(candidate);                             \
    }                                                           \
  } while (0)

  // Address the controller (/dev/nvme0), not the namespace (/dev/nvme0n1):
  // identity comes from an NVMe admin passthru, which is always permitted on
  // the controller and increasingly restricted on the namespace, and one
  // physical drive then yields exactly one row.
  for (const std::string & name : sysfsEntries("/sys/class/nvme")) {
    const std::string dir = "/sys/class/nvme/" + name;

    // Reject only what is definitively not a drive: NVMe-oF discovery
    // controllers and NVMe 2.0 administrative controllers.
    //
    // Do NOT require "io" here.  The kernel prints "reserved" whenever
    // Identify Controller byte 111 (CNTRLTYPE) reads 0, and that field only
    // exists as of NVMe 1.4 -- so every older drive reports 0 and shows up as
    // "reserved".
    std::string cntrltype;
    if (readSysfsAttribute(dir + "/cntrltype", cntrltype)
        && (cntrltype == "discovery" || cntrltype == "admin")) {
      LOG(D2) << "Not scanning " << dir << ": cntrltype is " << cntrltype;
      continue;
    }

    addCandidate(devrefForSysfsNode(dir, name), "NVMe controller " << name);
  }

  // /sys/block lists whole disks only -- partitions live underneath their
  // parent -- so partitions need no filtering.  loop, ram, zram, dm-, md, nbd
  // and zd have no `device' link at all; virtio-blk, mmcblk and NVMe
  // namespaces have one but no `type' under it.  All of them drop out here.
  for (const std::string & name : sysfsEntries("/sys/block")) {
    const std::string dir = "/sys/block/" + name;
    const std::string devicedir = dir + "/device";

    long type;
    if (!readSysfsDeviceType(devicedir, type)) {
      LOG(D2) << "Not scanning " << dir << ": no readable " << devicedir << "/type";
      continue;
    }
    if (type != SCSI_TYPE_DIRECT_ACCESS) {
      LOG(D2) << "Not scanning " << dir << ": device type is " << type
              << ", not " << SCSI_TYPE_DIRECT_ACCESS << " (direct access)";
      continue;
    }

    addCandidate(devrefForSysfsNode(dir, name), "direct-access block device " << name);
  }

  // Nothing on a plain SAS box matches here -- every disk has a block node,
  // and the sg-only nodes are the enclosures, which type 13 excludes.
  for (const std::string & name : sysfsEntries("/sys/class/scsi_generic")) {
    const std::string dir = "/sys/class/scsi_generic/" + name;
    const std::string devicedir = dir + "/device";

    long type;
    if (!readSysfsDeviceType(devicedir, type)) {
      LOG(D2) << "Not scanning " << dir << ": no readable " << devicedir << "/type";
      continue;
    }
    if (type != SCSI_TYPE_DIRECT_ACCESS) {
      LOG(D2) << "Not scanning " << dir << ": device type is " << type
              << ", not " << SCSI_TYPE_DIRECT_ACCESS << " (direct access)";
      continue;
    }
    if (sysfsHasEntries(devicedir + "/block")) {
      LOG(D2) << "Not scanning " << dir << ": already covered by its block device";
      continue;
    }

    addCandidate(devrefForSysfsNode(dir, name), "direct-access SCSI device " << name
                 << " with no block node");
  }

#undef addCandidate

  std::sort(devrefs.begin(), devrefs.end(), devrefNaturalLess);

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



// Use C++ SFINAE to create sd_device_new_from_devname if it doesn't exist.
// systemd 251+ has sd_device_new_from_devname(),
// systemd 248+ has sd_device_new_from_stat_rdev().
template <typename SD_DEVICE>
static int sd_device_new_from_devname(SD_DEVICE **ret, const char *devname) {
    struct stat st;

    if (stat(devname, &st) < 0)
        return -errno;

    return sd_device_new_from_stat_rdev(ret, &st);
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
    // Expected on anything that is not a block device -- an NVMe controller
    // char device always fails here -- and device_info.devSize is never read.
    LOG(D4) << "Failed to get device size for " << devref << ": " << strerror(errno);
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
#define showProperty(k)  do { std::string & prop=deviceProperties[k]; \
    LOG(D4) << k << " is " << (prop.empty() ? std::string("*** missing ***") : prop ); } while (0)
  // const char *value, *key;
  // FOREACH_DEVICE_PROPERTY(device, key, value) deviceProperties[key] = value ;
  const char *key, *value;
  for (key = sd_device_get_property_first(device, &value);
       key != NULL;
       key = sd_device_get_property_next(device, &value))
    {
      deviceProperties[key] = value ;
      IFLOG(D4) { showProperty(key); }
    }

  // Done with `sd_device device`
  sd_device_unref(device);

  LOG(D4) << std::endl;
  showProperty("ID_BUS");
  showProperty("DEVPATH");
  std::string bus=deviceProperties["ID_BUS"];
  std::string devpath=deviceProperties["DEVPATH"];
  LOG(D4) << std::endl;
  LOG(D4) << "bus is " << bus ;
  LOG(D4) << std::endl;


  if (bus=="scsi") {
    LOG(D3) << "device_info.devType = DEVICE_TYPE_SCSI because"
            << " bus is scsi";
    device_info.devType = DEVICE_TYPE_SCSI;
  } else if (bus == "usb") {
    if (deviceProperties["ID_USB_DRIVER"]=="uas") {
      LOG(D3) << "device_info.devType = DEVICE_TYPE_SAS because"
              << " bus is usb and ID_USB_DRIVER is uas";
      device_info.devType = DEVICE_TYPE_SAS;
    } else if (deviceProperties["ID_USB_DRIVER"]=="usb-storage") {
      LOG(D3) << "device_info.devType = DEVICE_TYPE_USB because"
              << " bus is usb and ID_USB_DRIVER is usb-storage";
      device_info.devType = DEVICE_TYPE_USB;
    }
  } else if (bus == "ata") {
    if (deviceProperties["ID_USB_DRIVER"]=="uas") {
      LOG(D3) << "device_info.devType = DEVICE_TYPE_USB because"
              << " bus is ata and ID_USB_DRIVER is uas";
      device_info.devType = DEVICE_TYPE_USB;
    } else {
      LOG(D3) << "device_info.devType = DEVICE_TYPE_ATA because"
              << " bus is ata and ID_USB_DRIVER is not uas";
      device_info.devType = DEVICE_TYPE_ATA;
    }
  } else if (bus == "nvme"
          || devpath.find("/nvme/") != std::string::npos) {
    LOG(D3) << "device_info.devType = DEVICE_TYPE_NVME because"
            << (bus == "nvme" ? std::string(" bus is nvme")
                : std::string(" \"/nvme/\" is a substring of devpath=")+devpath);
    device_info.devType = DEVICE_TYPE_NVME;
  } else if (bus.empty() && deviceProperties["SUBSYSTEM"] == "scsi_generic") {
    // udev's persistent-storage rules run on block devices, so an sg node has
    // no ID_BUS at all.  Without this a HBA-claimed disk reached by its sg
    // node falls through to DEVICE_TYPE_OTHER and is dropped.
    long type;
    if (readSysfsDeviceType("/sys" + devpath + "/device", type)
        && type == SCSI_TYPE_DIRECT_ACCESS) {
      LOG(D3) << "device_info.devType = DEVICE_TYPE_SCSI because"
              << " it is the scsi_generic node of a direct-access device";
      device_info.devType = DEVICE_TYPE_SCSI;
    }
  }

  // Copy device properties from `deviceProperties` into `device_info`
#define getDeviceProperty(key,field)                                    \
  do                                                                    \
    if (1==deviceProperties.count(#key)) {                              \
      std::string deviceProperty(deviceProperties[#key]);               \
      showProperty(#key);                                               \
      safecopy(device_info.field, sizeof(device_info.field), (uint8_t *)deviceProperty.c_str(), strlen(deviceProperty.c_str())); \
    } while (0)

  LOG(D4) << "Device properties from linux:";
  getDeviceProperty(ID_SERIAL_SHORT,serialNum) ;
  getDeviceProperty(ID_MODEL,modelNum) ;
  getDeviceProperty(ID_REVISION,firmwareRev) ;
  getDeviceProperty(ID_VENDOR,vendorID) ;


  std::string str_WWN("");
  size_t WWN_length=0;
  if (1==deviceProperties.count("ID_WWN")) {
    str_WWN=deviceProperties["ID_WWN"];
    LOG(D4) << "ID_WWN is " << str_WWN;
    std::transform(str_WWN.begin(), str_WWN.end(), str_WWN.begin(), ::toupper);
    LOG(D4) << "ID_WWN in uppercase is " << str_WWN;
    // Various WWN hacks
    // Might start with "0X"
    if (str_WWN.substr(0,2)=="0X") {
        str_WWN=str_WWN.substr(2);
    // Might start with "EUI."
    } else if (str_WWN.substr(0,4)=="EUI.") {
        str_WWN=str_WWN.substr(4);
    // Might start with "NVME."
    } else if (str_WWN.substr(0,5)=="NVME.") {
        str_WWN="";
    }
    WWN_length=str_WWN.length();
  }
  if (0 < WWN_length) {
    LOG(D4) << "str_WWN is " << str_WWN;
    LOG(D4) << "str_WWN.length()=" << str_WWN.length();

    size_t device_info_nybbles = 2 * sizeof(device_info.worldWideName);
    LOG(D4) << "device_info_nybbles=" << device_info_nybbles;


    if (device_info.devType == DEVICE_TYPE_NVME)  {
      intptr_t length_difference =  device_info_nybbles - WWN_length ;
      LOG(D4) << "length_difference=" << length_difference;

      if (0 < length_difference) {
        str_WWN += std::string(length_difference, '0');
      } else {
        str_WWN = str_WWN.substr(-length_difference, device_info_nybbles );
      }
    } else {
      intptr_t length_difference =  device_info_nybbles - 1 - WWN_length ;
      LOG(D4) << "length_difference=" << length_difference;

      if (0 < length_difference) {
        str_WWN += std::string(length_difference, '0');
      } else {
        str_WWN = str_WWN.substr(-length_difference, device_info_nybbles - 1);
      }
      str_WWN =std::string(1, '5' ) + str_WWN;
    }


    LOG(D4) << "str_WWN is " << str_WWN;
    LOG(D4) << "str_WWN.length()=" << str_WWN.length();
    LOG(D4) << "sizeof(device_info.worldWideName)=" << sizeof(device_info.worldWideName);
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
  // serialNum is bigger so just copy the first part
  memcpy(device_info.passwordSalt, device_info.serialNum, sizeof(device_info.passwordSalt));


  // Copy `device_info` fields into `interfaceDeviceIndentification` blob for special cases
  uint8_t * p = (uint8_t *)interfaceDeviceIdentification;
#define copyDeviceIdentificationField(field,size)                       \
  do { memcpy(p,device_info.field, size); p += size; } while (0)

  copyDeviceIdentificationField(vendorID,INQUIRY_VENDOR_IDENTIFICATION_Length);
  copyDeviceIdentificationField(modelNum,INQUIRY_PRODUCT_IDENTIFICATION_Length);
  copyDeviceIdentificationField(firmwareRev,INQUIRY_PRODUCT_REVISION_LEVEL_Length);

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
  LOG(D4) << " DtaLinux::PerformSCSICommand ioctl kernResult=" << HEXON(8) << kernResult ;
  if (0 != kernResult) {
    LOG(E) << "DtaLinux::PerformSCSICommand:: ioctl(" << handleDescriptor(osDeviceHandle)
           << ", ...) failed with kernResult " << HEXON(8) << kernResult;
    LOG(E) << "DtaLinux::PerformSCSICommand:: sg.masked_status=" << (int)sg.masked_status;
    IFLOG(D4) {
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

  return kernResult ;
}


/** Perform a NVMe command using the Linux `nvme_admin_cmd' (NVMe standard) interface with ioctl `NVME_IOCTL_ADMIN_CMD'
 *
 * @param osDeviceHandle  OSDEVICEHANDLE of already-opened raw device file
 * @param cmd             NVMe command struct
 *
 * Returns the result of the os system call, as well as possibly setting *pstatus
 */
int DtaLinux::PerformNVMeCommand(OSDEVICEHANDLE osDeviceHandle,
                                 uint8_t * pcmd,
                                 uint32_t * pstatus)
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
  LOG(D4) << " DtaLinux::PerformNVMeCommand ioctl kernResult=" << HEXON(8) << kernResult ;

  if (0 != kernResult) {
    IFLOG(D4) {
      LOG(D4) << "DtaLinux::PerformNVMeCommand:: ioctl(osDeviceHandle=" << handleDescriptor(osDeviceHandle)
              << ", ...) failed with kernResult " << HEXON(8) << kernResult << " (" << strerror(kernResult) << ")" ;
      LOG(D4) << " cmd:" ;
      DtaHexDump(&cmd, sizeof(cmd));
    }
  }

  if (0 == kernResult)
    (*pstatus) = cmd.result;

  return kernResult ;
}
