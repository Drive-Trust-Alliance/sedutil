AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CFLAGS = -Wall -Werror -std=c11
AM_CXXFLAGS = -Wall -Werror -std=c++11 -I./linux \
              -I$(srcdir)/Common -I$(srcdir)/Common/pbkdf2 -I$(srcdir)/linux -I$(srcdir)/LinuxPBA

SEDUTIL_COMMON_CODE = \
Common/ATAStructures.h \
Common/DtaAnnotatedDump.cpp \
Common/DtaAnnotatedDump.h \
Common/DtaAtaDrive.cpp \
Common/DtaAtaDrive.h \
Common/DtaAuthorizeSedutilExecution.h \
Common/DtaBlockStorageDevice.cpp \
Common/DtaBlockStorageDevice.h \
Common/DtaCommand.cpp \
Common/DtaCommand.h \
Common/DtaConstants.h \
Common/DtaDev.cpp \
Common/DtaDev.h \
Common/DtaDevEnterprise.cpp \
Common/DtaDevEnterprise.h \
Common/DtaDevFactory.cpp \
Common/DtaDevGeneric.cpp \
Common/DtaDevGeneric.h \
Common/DtaDevOpal1.cpp \
Common/DtaDevOpal1.h \
Common/DtaDevOpal2.cpp \
Common/DtaDevOpal2.h \
Common/DtaDevOpal.cpp \
Common/DtaDevOpal.h \
Common/DtaDrive.cpp \
Common/DtaDrive.h \
Common/DtaEndianFixup.h \
Common/DtaHashPassword.cpp \
Common/DtaHashPassword.h \
Common/DtaHashPwd.cpp \
Common/DtaHashPwd.h \
Common/DtaHexDump.cpp \
Common/DtaHexDump.h \
Common/DtaLexicon.h \
Common/DtaNvmeDrive.cpp \
Common/DtaNvmeDrive.h \
Common/DtaOS.h \
Common/DtaResponse.cpp \
Common/DtaResponse.h \
Common/DtaSataDrive.cpp \
Common/DtaSataDrive.h \
Common/DtaScsiDrive.cpp \
Common/DtaScsiDrive.h \
Common/DtaSession.cpp \
Common/DtaSession.h \
Common/DtaStructures.h \
Common/InterfaceDeviceID.cpp \
Common/InterfaceDeviceID.h \
Common/log.cpp \
Common/log.h \
Common/ParseATIdentify.cpp \
Common/ParseATIdentify.h \
Common/ParseDiscovery0Features.cpp \
Common/ParseDiscovery0Features.h \
Common/SCSICmds_INQUIRY_Definitions.h

SEDUTIL_LINUX_CODE = \
linux/db.h \
linux/os.h \
linux/DtaLinux.cpp \
linux/DtaLinux.h \
linux/DtaLinuxDriveFactory.cpp \
linux/DtaLinuxAtaDrive.cpp \
linux/DtaLinuxAtaDrive.h \
linux/DtaLinuxBlockStorageDevice.cpp \
linux/DtaLinuxBlockStorageDevice.h \
linux/DtaLinuxNvmeDrive.cpp \
linux/DtaLinuxNvmeDrive.h \
linux/DtaLinuxSataDrive.cpp \
linux/DtaLinuxSataDrive.h \
linux/DtaLinuxScsiDrive.cpp \
linux/DtaLinuxScsiDrive.h

SEDUTIL_CUSTOMIZATIONS_COMMON_CODE = \
Customizations/Common/DtaAuthorizeSedutilExecution.cpp \
Customizations/Common/DtaExtensionOptionEnumerations.h \
Customizations/Common/DtaDevOpalLoadPBA.cpp

SEDUTIL_CIFRA_CODE = \
Common/pbkdf2/blockwise.h \
Common/pbkdf2/blockwise.c \
Common/pbkdf2/sha1.h \
Common/pbkdf2/sha1.c \
Common/pbkdf2/pbkdf2.h \
Common/pbkdf2/pbkdf2.c \
Common/pbkdf2/chash.h \
Common/pbkdf2/chash.c \
Common/pbkdf2/hmac.h  \
Common/pbkdf2/hmac.c \
Common/pbkdf2/bitops.h \
Common/pbkdf2/handy.h \
Common/pbkdf2/tassert.h

sbin_PROGRAMS = sedutil-cli linuxpba

sedutil_cli_SOURCES = \
Common/DtaOptions.cpp \
Common/DtaOptions.h \
$(SEDUTIL_LINUX_CODE) \
$(SEDUTIL_COMMON_CODE) \
$(SEDUTIL_CUSTOMIZATIONS_COMMON_CODE) \
$(SEDUTIL_LOG_CODE) \
$(SEDUTIL_CIFRA_CODE) \
Common/sedutil.cpp

LDADD = -lsystemd

CLEANFILES = linux/Version.h

BUILT_SOURCES = linux/Version.h
#

linuxpba_SOURCES = \
LinuxPBA/GetPassPhrase.cpp \
LinuxPBA/GetPassPhrase.h \
LinuxPBA/UnlockSEDs.cpp \
LinuxPBA/UnlockSEDs.h \
$(SEDUTIL_LINUX_CODE) \
$(SEDUTIL_COMMON_CODE) \
$(SEDUTIL_CUSTOMIZATIONS_COMMON_CODE) \
$(SEDUTIL_LOG_CODE) \
$(SEDUTIL_CIFRA_CODE) \
LinuxPBA/LinuxPBA.cpp

EXTRA_DIST = \
Customizations/linux/CLI/GitVersion.sh \
linux/PSIDRevert_LINUX.txt \
linux/TestSuite.sh \
README.md docs/sedutil-cli.8

man_MANS = docs/sedutil-cli.8

linux/Version.h:
	$(srcdir)/Customizations/linux/CLI/GitVersion.sh > linux/Version.h

.INTERMEDIATE: linux/Version.h

maintainer-clean-local:
	rm config.status configure config.log
	rm Makefile
	rm -r autom4te.cache/
	rm aclocal.m4
	rm compile install-sh missing Makefile.in
	rm -rf depcomp
