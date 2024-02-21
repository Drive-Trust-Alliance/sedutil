AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CFLAGS = -Wall -Werror -std=c11
AM_CXXFLAGS = -Wall -Werror -std=c++11 -I./linux -I$(srcdir)/Common -I$(srcdir)/Common/pbkdf2 -I$(srcdir)/linux -I$(srcdir)/LinuxPBA
SEDUTIL_COMMON_CODE = \
Common/ATAStructures.h \
Common/DtaAnnotatedDump.cpp \
Common/DtaAnnotatedDump.h \
Common/DtaAuthorizeSedutilExecution.h \
Common/DtaCommand.cpp \
Common/DtaCommand.h \
Common/DtaDev.cpp \
Common/DtaDevEnterprise.cpp \
Common/DtaDevEnterprise.h \
Common/DtaDevGeneric.cpp \
Common/DtaDevGeneric.h \
Common/DtaDev.h \
Common/DtaDevOpal1.cpp \
Common/DtaDevOpal1.h \
Common/DtaDevOpal2.cpp \
Common/DtaDevOpal2.h \
Common/DtaDevOpal.cpp \
Common/DtaDevOpal.h \
Common/DtaEndianFixup.h \
Common/DtaHashPassword.cpp \
Common/DtaHashPassword.h \
Common/DtaHashPwd.cpp \
Common/DtaHashPwd.h \
Common/DtaHexDump.cpp \
Common/DtaHexDump.h \
Common/DtaLexicon.h \
Common/DtaResponse.cpp \
Common/DtaResponse.h \
Common/DtaSession.cpp \
Common/DtaSession.h \
Common/ParseATIdentify.h \
Common/ParseATIdentify.cpp \
Common/InterfaceDeviceID.h \
Common/InterfaceDeviceID.cpp \
Common/DtaStructures.h

SEDUTIL_LINUX_CODE = \
linux/DtaConstants.h \
linux/os.h \
linux/DtaDevOS.h \
linux/DtaDevOS.cpp \
linux/DtaDevFactory.cpp \
linux/DtaDevLinuxDrive.h \
linux/DtaDevLinuxDrive.cpp \
linux/DtaDevLinuxNvme.h \
linux/DtaDevLinuxNvme.cpp \
linux/DtaDevLinuxScsi.h \
linux/DtaDevLinuxScsi.cpp \
linux/DtaDevLinuxSata.h \
linux/DtaDevLinuxSata.cpp

SEDUTIL_CUSTOMIZATIONS_COMMON_CODE = \
Customizations/Common/DtaAuthorizeSedutilExecution.cpp \
Customizations/Common/DtaExtensionOptionEnumerations.h \
Customizations/Common/DtaDevOpalLoadPBA.cpp

SEDUTIL_LOG_CODE = \
log/log.h \
log/log.cpp

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
        Common/sedutil.cpp Common/DtaOptions.cpp \
	Common/DtaOptions.h \
	$(SEDUTIL_LINUX_CODE) \
	$(SEDUTIL_COMMON_CODE) \
	$(SEDUTIL_CUSTOMIZATIONS_COMMON_CODE) \
	$(SEDUTIL_LOG_CODE) \
	$(SEDUTIL_CIFRA_CODE)

CLEANFILES = linux/Version.h

BUILT_SOURCES = linux/Version.h
#

linuxpba_SOURCES = LinuxPBA/GetPassPhrase.cpp LinuxPBA/UnlockSEDs.cpp \
	LinuxPBA/GetPassPhrase.h LinuxPBA/UnlockSEDs.h \
	$(SEDUTIL_LINUX_CODE) \
	$(SEDUTIL_COMMON_CODE) \
	$(SEDUTIL_CUSTOMIZATIONS_COMMON_CODE) \
	$(SEDUTIL_LOG_CODE) \
	$(SEDUTIL_CIFRA_CODE) \
	LinuxPBA/LinuxPBA.cpp

EXTRA_DIST = linux/GitVersion.sh linux/PSIDRevert_LINUX.txt linux/TestSuite.sh README.md docs/sedutil-cli.8

man_MANS = docs/sedutil-cli.8

linux/Version.h:
	$(srcdir)/linux/GitVersion.sh > linux/Version.h

.INTERMEDIATE: linux/Version.h

maintainer-clean-local:
	rm config.status configure config.log
	rm Makefile
	rm -r autom4te.cache/
	rm aclocal.m4
	rm compile install-sh missing Makefile.in
	rm -rf depcomp
