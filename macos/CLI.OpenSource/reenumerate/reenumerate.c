#include <CoreFoundation/CoreFoundation.h>

#include <getopt.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>

#include "IOUSBFamilyInfoPlist.pch"

//—————————————————————————————————————————————————————————————————————————————
//    Globals
//—————————————————————————————————————————————————————————————————————————————

const char *        gProgramName                = NULL;
boolean_t           gVerbose                    = FALSE;
boolean_t           gDoReenumeration            = TRUE;
boolean_t           gDoSetConfiguration         = FALSE;
boolean_t           gDoSuspend                  = FALSE;
boolean_t           gDoResume                   = FALSE;
boolean_t           gDoReset                    = FALSE;
UInt8               gConfiguration              = 0;

#define    vlog(x...)                    if ( gVerbose ) { fprintf(stdout,x); }
#define    elog(x...)                    fprintf(stderr, x)

//———————————————————————————————————————————————————————————————————————————
//    Prototypes
//———————————————————————————————————————————————————————————————————————————

static void ParseArguments ( int argc, const char * argv[] );
void PrintUsage ( void );

//———————————————————————————————————————————————————————————————————————————
//    PrintUsage
//———————————————————————————————————————————————————————————————————————————

void
PrintUsage ( void )
{
    elog ( "\n");
    elog ( "Usage: %s [OPTIONS] vendor_id,product_id [vendor_id,product_id]...\n", gProgramName );
    elog ( "\n");
    
    elog ( "OPTIONS\n");
    elog ( "\tThe available options are as follows.  If no option is specified, a reenumerate command will be send to the device(s):\n");
    elog ( "\n");
    
    
    elog ( "\t-c configuration");
    elog ( "\t\t Set the USB configuration to the value specified.\n");
    
    elog ( "\t-r");
    elog ( "\t\t Send a USB Resume to the device.\n");
    
    elog ( "\t-s");
    elog ( "\t\t Send a USB Suspend to the device.\n");
    
    elog ( "\t-R");
    elog ( "\t\t Send a USB ResetDevice to the device.\n");
    
    elog ( "\t--verbose, -v\n");
    elog ( "\t\t Verbose mode.\n");
    
    elog ( "\t--version, -V\n");
    elog ( "\t\t Print version.\n");
    
    elog ( "\t--help, -h, -?\n");
    elog ( "\t\t Show this help.\n");

    exit ( 0 );
    
}


//———————————————————————————————————————————————————————————————————————————
//    ParseArguments
//———————————————————————————————————————————————————————————————————————————

static void
ParseArguments ( int argc, const char * argv[] )
{
    int                 c;
    struct option       long_options[] =
    {
        { "reset",      no_argument,        0, 'R' },
        { "suspend",    no_argument,        0, 's' },
        { "resume",     no_argument,        0, 'r' },
        { "configure",  required_argument,  0, 'c' },
        
        { "verbose",    no_argument,        0, 'v' },
        { "version",    no_argument,        0, 'V' },
        { "help",       no_argument,        0, 'h' },
        { 0, 0, 0, 0 }
    };
    
    if ( argc == 1 )
    {
        return;
    }
    
    while ( ( c = getopt_long ( argc, ( char * const * ) argv , "Rsrc:vVh?", long_options, NULL  ) ) != -1 )
    {
        switch ( c )
        {
            case 'R':
                gDoReset = TRUE;
                gDoReenumeration = FALSE;
                break;
                
            case 's':
                gDoSuspend = TRUE;
                gDoReenumeration = FALSE;
                break;
                
            case 'r':
                gDoResume = TRUE;
                gDoReenumeration = FALSE;
                break;
                
            case 'c':
                gDoSetConfiguration = TRUE;
                gConfiguration = (UInt8)strtoul(optarg, NULL, 0);    // is this safe?
                break;
                
                
            case 'v':
                gVerbose = TRUE;
                vlog( "Verbose mode ON\n");
                break;
                
            case 'V':
                fprintf(stdout,"%s version:  %s\n", gProgramName, QUOTEDSTRING(USBTRACE_VERSION));
                break;
                
            case 'h':
                PrintUsage ( );
                break;
                
            case '?':
                PrintUsage ( );
                break;
                
            default:
                break;
        }
    }
}

static
void ProcessDevice(io_service_t aDevice)
{
    IOCFPlugInInterface     **plugInInterface;
    IOUSBDeviceInterface187 **deviceInterface;
    SInt32                  score;
    HRESULT                 res;
    CFNumberRef             numberObj;
    io_name_t               name;
    uint32_t                locationID = 0;
    IOReturn                kr;
    
    kr = IORegistryEntryGetName(aDevice, name);
    if ( kr != kIOReturnSuccess)
        return;
    
    numberObj = IORegistryEntryCreateCFProperty(aDevice, CFSTR("locationID"), kCFAllocatorDefault, 0);
    if ( numberObj != NULL )
    {
        CFNumberGetValue(numberObj, kCFNumberSInt32Type, &locationID);
        CFRelease(numberObj);
        
        vlog("Found \"%s\" @ 0x%8.8x\n", name, locationID);
        
    }
    
    kr = IOCreatePlugInInterfaceForService(aDevice, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
    IOObjectRelease(aDevice);
    if ((kIOReturnSuccess != kr) || !plugInInterface) {
        elog("IOCreatePlugInInterfaceForService returned 0x%08x.\n", kr);
        return;
    }
    
    // Use the plugin interface to retrieve the device interface.
    res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID187),
                                             (LPVOID *)&deviceInterface);
    // Now done with the plugin interface.
    (*plugInInterface)->Release(plugInInterface);
    
    if (0 != res || deviceInterface == NULL) {
        fprintf(stderr, "QueryInterface returned %d.\n", (int) res);
        return;
    }
    
    kr = (*deviceInterface)->USBDeviceOpen(deviceInterface);
    if (kr == kIOReturnSuccess)
    {
        if (gDoSetConfiguration)
        {
            vlog("Calling SetConfiguration(%d)\n", gConfiguration);
            kr = (*deviceInterface)->SetConfiguration(deviceInterface, gConfiguration);
            vlog("SetConfiguration(%d) returns 0x%8.8x\n", (uint32_t)gConfiguration, kr);
        }
        else if (gDoSuspend)
        {
            vlog("Calling USBDeviceSuspend(TRUE)\n");
        kr = (*deviceInterface)->USBDeviceSuspend(deviceInterface, TRUE);
            vlog("USBDeviceSuspend(TRUE) returns 0x%8.8x\n", kr);
        }
        else if (gDoResume)
        {
            vlog("Calling USBDeviceSuspend(FALSE)\n");
            kr = (*deviceInterface)->USBDeviceSuspend(deviceInterface, FALSE);
            vlog("USBDeviceSuspend(FALSE) returns 0x%8.8x\n", kr);
        }
        else if (gDoReset)
        {
            vlog("Calling ResetDevice\n");
            kr = (*deviceInterface)->ResetDevice(deviceInterface);
            vlog("ResetDevice returns 0x%8.8x\n", kr);
        }
        else
        {
            vlog("Calling USBDeviceReEnumerate\n");
            kr = (*deviceInterface)->USBDeviceReEnumerate(deviceInterface, 0);
            vlog("USBDeviceReEnumerate returns 0x%8.8x\n", kr);
        }
    }
    
    (void) (*deviceInterface)->USBDeviceClose(deviceInterface);
    
    (*deviceInterface)->Release(deviceInterface);
}

static
mach_port_t default_port (void);
static mach_port_t default_port (void)
{
#if defined (MAC_OS_VERSION_12_0) && MAC_OS_VERSION_12_0 <= MAC_OS_X_VERSION_MAX_ALLOWED
    if (__builtin_available(macOS 12.0, *))
    {
        return kIOMainPortDefault;
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
    return kIOMasterPortDefault;
#if defined (MAC_OS_VERSION_12_0) && MAC_OS_VERSION_12_0 <= MAC_OS_X_VERSION_MAX_ALLOWED
#pragma clang diagnostic pop
#endif
}


//================================================================================================
//    main
//================================================================================================
int main( int argc, const char *argv[] )
{
    CFMutableDictionaryRef  matchingDict;
    CFNumberRef             numberRef;
    kern_return_t           kr;
    uint32_t                usbVendor;
    uint32_t                usbProduct;
    const char *            param;
    char *                  param2;
    int                     paramIndex = 1;
    io_iterator_t           foundDevices;
    io_object_t             aDevice;
    
     gProgramName = argv[0];

    // Get program arguments.
    ParseArguments ( argc, argv );

    for( paramIndex = optind ; paramIndex < argc ; paramIndex++ )
    {
        param = argv[paramIndex];
        
        usbVendor = (uint32_t) strtoul(param, &param2, 0);
        usbProduct = *param2++ ? (uint32_t) strtoul(param2, 0, 0) : 0;
        
        vlog("Looking for vid: 0x%x, pid: 0x%x\n", (uint32_t)usbVendor, (uint32_t)usbProduct);
        
        matchingDict = IOServiceMatching(kIOUSBDeviceClassName);    // Interested in instances of class
                                                                    // IOUSBDevice and its subclasses
        if (matchingDict == NULL) {
            elog("IOServiceMatching returned NULL.\n");
            return -1;
        }
        
        numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbVendor);
        CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorID), numberRef);
        CFRelease(numberRef);
    
        numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbProduct);
        CFDictionarySetValue(matchingDict, CFSTR(kUSBProductID),  numberRef);
        CFRelease(numberRef);
     
        kr = IOServiceGetMatchingServices(default_port(), matchingDict, &foundDevices);    //consumes matchingDIct reference
        if ( 0 != kr)
        {
            elog("Error 0x%x trying to find matching services\n", kr);
            continue;
        }
        
        
        while ((aDevice = IOIteratorNext(foundDevices)))
        {
            ProcessDevice(aDevice);
        }
        
    }
    
    return 0;
}
