BEGIN_OPTION_MAX(scan, 1)
    TESTOPTIONALARG(--json, output_format, sedutilJSON)
    TESTOPTIONALARG(--json-compact, output_format, sedutilJSONCompact)
    ENDOPTIONALARGS
    USAGE_IS(R"-(
--scan
                                Scans the devices on the system
                                identifying the TPers
--scan --json
                                Scans the devices on the system
                                identifying the TPers; output in JSON
--scan --json-compact
                                Scans the devices on the system
                                identifying the TPers; output in compact JSON

)-")
        END_OPTION


BEGIN_OPTION(initialSetup, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--initialSetup <SIDpassword> <device>
                                Setup the device for use with sedutil
                                <SIDpassword> is new SID and Admin1 password
)-")
        END_OPTION

BEGIN_OPTION(isValidSED, 1) OPTION_IS(device)
    USAGE_IS(R"-(
--isValidSED <device>
                                Verify whether the given device is SED or not
)-")
END_OPTION

BEGIN_OPTION_MAX(query, 2)
    TESTOPTIONALARG(--json, output_format, sedutilJSON)
    TESTOPTIONALARG(--json-compact, output_format, sedutilJSONCompact)
    ENDOPTIONALARGS
    OPTION_IS(device)
    USAGE_IS(R"-(
--query <device>
                                Display the complete device information, including Discovery 0 response
--query --json <device>
                                Display the complete device information, including Discovery 0 response, in JSON format
--query --json-compact <device>
                                Display the complete device information, including the Discovery 0 response, in compact JSON format
)-")
END_OPTION

BEGIN_OPTION(listLockingRanges, 2)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--listLockingRanges <password> <device>
                                List all Locking Ranges
)-")
END_OPTION

BEGIN_OPTION(listLockingRange, 3)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--listLockingRange <lockingRange< <password> <device>
                                List specific Locking Range
                                0 = GLobal   1..n = LRn
)-")
END_OPTION

BEGIN_OPTION(rekeyLockingRange, 3)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--rekeyLockingRange <lockingRange> <password> <device>
                                0 = GLobal   1..n = LRn
                                Rekey Locking Range
)-")
END_OPTION

BEGIN_OPTION(setBandsEnabled, 2)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setBandsEnabled <password> <device>
                                Set Enabled for all Locking Ranges
                                (password = "" for MSID)
)-")
END_OPTION

BEGIN_OPTION(setBandEnabled, 3)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setBandEnabled <0...n> <password> <device>
                                Set Enabled for Locking Range[n]
                                (password = "" for MSID)
)-")
END_OPTION

BEGIN_OPTION(eraseLockingRange, 3)
            LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--eraseLockingRange <0...n> <password> <device>
                                Erase a Locking Range
                                0 = GLobal  1..n = LRn
)-")
END_OPTION

BEGIN_OPTION(eraseLockingRange_SUM, 3)
            LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--eraseLockingRange_SUM <0...n> <password> <device>
                                Erase a Locking Range Single User Mode
                                0 = GLobal  1..n = LRn
)-")
END_OPTION

BEGIN_OPTION(setupLockingRange, 5)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(lrstart)
    OPTION_IS(lrlength)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setupLockingRange <0...n> <RangeStart> <RangeLength> <password> <device>
                                Setup a new Locking Range
                                0 = GLobal 1..n  = LRn
)-")
END_OPTION

BEGIN_OPTION(setupLockingRange_SUM, 5)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(lrstart)
    OPTION_IS(lrlength)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setupLockingRange_SUM <0...n> <RangeStart> <RangeLength> <password> <device>
                                Setup a new Locking Range Single User Mode
                                0 = GLobal  1..n = LRn
)-")
END_OPTION


BEGIN_OPTION(setSIDPassword, 3) OPTION_IS(password) OPTION_IS(newpassword)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setSIDPassword <SIDpassword> <newSIDpassword> <device>
                                Change the SID password
)-")
    END_OPTION

BEGIN_OPTION(setAdmin1Pwd, 3)
    OPTION_IS(password)
    OPTION_IS(newpassword)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setAdmin1Pwd <Admin1password> <newAdmin1password> <device>
                                Change the Admin1 password
)-")
END_OPTION

BEGIN_OPTION(setPassword, 4)
    OPTION_IS(password)
    OPTION_IS(userid)
    OPTION_IS(newpassword)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setPassword <oldpassword, \"\" for MSID> <userid> <newpassword> <device>
                                Change the Enterprise password for userid
                                \"EraseMaster\" or \"BandMaster<n>\", 0 <= n <= 1023
)-")
END_OPTION

BEGIN_OPTION(setPassword_SUM, 4)
    OPTION_IS(password)
    OPTION_IS(userid)
    OPTION_IS(newpassword)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setPassword_SUM <oldpassword, \"\" for MSID> <userid> <newpassword> <device>
                                Change the Enterprise password for userid Single User Mode
                                \"EraseMaster\" or \"BandMaster<n>\", 0 <= n <= 1023
)-")
END_OPTION

BEGIN_OPTION(setLockingRange, 4)
    LOCKINGRANGEARG(lockingrange)
            LOCKINGSTATEARG(lockingstate)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setLockingRange <0...n> <RW|RO|LK> <Admin1password> <device>
                                Set the status of a Locking Range
                                0 = GLobal 1..n  = LRn
)-")
END_OPTION

BEGIN_OPTION(setLockingRange_SUM, 4)
    LOCKINGRANGEARG(lockingrange)
            LOCKINGSTATEARG(lockingstate)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setLockingRange_SUM <0...n> <RW|RO|LK> <Admin1password> <device>
                                Set the status of a Locking Range Single User Mode
                                0 = GLobal 1..n  = LRn
)-")
END_OPTION

BEGIN_OPTION(enableLockingRange, 3)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--enableLockingRange <0...n> <Admin1password> <device>
                                Enable a Locking Range
                                0 = GLobal 1..n  = LRn
)-")
END_OPTION

BEGIN_OPTION(disableLockingRange, 3)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--disableLockingRange <0...n> <Admin1password> <device>
                                Disable a Locking Range
                                0 = GLobal 1..n  = LRn
)-")
END_OPTION

BEGIN_OPTION(setMBREnable, 3)
    MBRSTATEARG(MBREnable,mbrstate)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setMBREnable <on|off> <Admin1password> <device>
                                Enable|Disable MBR shadowing
)-")
END_OPTION

BEGIN_OPTION(setMBRDone, 3)
    MBRSTATEARG(MBRDone,mbrstate)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setMBRDone <on|off> <Admin1password> <device>
                                set|unset MBRDone
)-")
END_OPTION

BEGIN_OPTION(loadPBAimage, 3)
    OPTION_IS(password)
    OPTION_IS(pbafile)
    OPTION_IS(device)
    USAGE_IS(R"-(
--loadPBAimage <Admin1password> <file> <device>
                                Write <file> to MBR Shadow area
)-")
END_OPTION

BEGIN_OPTION(revertTPer, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--revertTPer <SIDpassword> <device>
                                set the device back to factory defaults
                                This **ERASES ALL DATA**
)-")
END_OPTION

BEGIN_OPTION(revertNoErase, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--revertNoErase <Admin1password> <device>
                                deactivate the Locking SP
                                without erasing the data
                                on GLOBAL RANGE *ONLY*
)-")
END_OPTION

BEGIN_OPTION(yesIreallywanttoERASEALLmydatausingthePSID, 2)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--yesIreallywanttoERASEALLmydatausingthePSID <PSID> <device>
                                revert the device using the PSID *ERASING* *ALL* the data
)-")
END_OPTION
BEGIN_OPTION(PSIDrevert, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--PSIDrevert <PSID> <device>
                                revert the device using the PSID *ERASING* *ALL* the data
)-")
END_OPTION

BEGIN_OPTION(PSIDrevertAdminSP, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--PSIDrevertAdminSP <PSID> <device>
                                revert the device Admin SP using the PSID
)-")
END_OPTION

BEGIN_OPTION(printDefaultPassword, 1) OPTION_IS(device)
    USAGE_IS(R"-(
--printDefaultPassword <device>
                                print MSID
)-")
END_OPTION


BEGIN_OPTION(setup_SUM, 6)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(lrstart)
    OPTION_IS(lrlength)
    OPTION_IS(password)
    OPTION_IS(newpassword)
    OPTION_IS(device)
    USAGE_IS(R"-(
--setup_SUM <lockingrange> <lrstart> <lrlength> <adminPassword> <newUserPassword> device>
                                Set up Single User Mode (User will be LR+1, i.e. LR2 will be User3)
)-")
END_OPTION

BEGIN_OPTION(activate, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--activate <newSIDPassword> <device>
                                Try to set new SID password or activate with default MSID
)-")
END_OPTION

BEGIN_OPTION(getmfgstate, 1) OPTION_IS(device)
    USAGE_IS(R"-(
--getmfgstate <device>
                                Get Locking SP life cycle state
)-")
END_OPTION


BEGIN_OPTION(enableuser, 4)
    MBRSTATEARG(enableuser,mbrstate)
    OPTION_IS(password)
    OPTION_IS(userid)
    OPTION_IS(device)
    USAGE_IS(R"-(
--enableuser <mbrstate> <password> <userid> <device>
)-")
END_OPTION
BEGIN_OPTION(enableuserread, 4)
    MBRSTATEARG(enableuser,mbrstate)
    OPTION_IS(password)
    OPTION_IS(userid)
    OPTION_IS(device)
    USAGE_IS(R"-(
--enableuser <mbrstateread> <password> <userid> <device>
)-")
END_OPTION


BEGIN_OPTION(activateLockingSP, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--activateLockingSP <password> <device>
)-")
END_OPTION

BEGIN_OPTION(activateLockingSP_SUM, 3)
            LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--activateLockingSP_SUM <password> <device>
)-")
END_OPTION


BEGIN_OPTION(takeOwnership, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--takeOwnership <password> <device>
)-")
END_OPTION

BEGIN_OPTION(revertLockingSP, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--revertLockingSP <password> <device>
)-")
END_OPTION

BEGIN_OPTION(validatePBKDF2, 0)
    USAGE_IS(R"-(
--validatePBKDF2
)-")
END_OPTION

BEGIN_OPTION(TCGreset, 2)
    TCGRESETTYPEARG(resettype)
    OPTION_IS(device)
    USAGE_IS(R"-(
--TCGReset <resettype> <device>
)-")
END_OPTION

BEGIN_OPTION(readonlyLockingRange, 3)
    LOCKINGRANGEARG(lockingrange)
    OPTION_IS(password)
    OPTION_IS(device)
    USAGE_IS(R"-(
--readonlyLockingRange <lockingRange> <password> <device>
)-")
END_OPTION

BEGIN_OPTION(objDump, 5)
    SKIP_FIELDS(4)
    OPTION_IS(device)
END_OPTION
    USAGE_IS(R"-(
--objDump <SP_OPALUID_Table_Index>  <auth>  <password>  <objUID> <device>
)-")

BEGIN_OPTION(rawCmd, 7)
    SKIP_FIELDS(6)
    OPTION_IS(device)
END_OPTION
BEGIN_OPTION(hashvalidation, 2) OPTION_IS(password) OPTION_IS(device)
    USAGE_IS(R"-(
--rawCmd <SP_OPALUID_Table_Index>  <auth>  <password>  <invoker> <method> <parameter list> <device>
)-")
END_OPTION

BEGIN_OPTION(version, 0)
    USAGE_IS(R"-(
)-")
  END_OPTION
