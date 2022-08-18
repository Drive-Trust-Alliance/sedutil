AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CFLAGS = -Wall -Werror -std=c11
AM_CXXFLAGS = -Wall -Werror -std=c++11 -I./linux -I$(srcdir)/Common -I$(srcdir)/Common/pbkdf2 -I$(srcdir)/linux -I$(srcdir)/LinuxPBA
SEDUTIL_COMMON_CODE = Common/log.h \
	Common/DtaEndianFixup.h Common/DtaStructures.h \
	Common/DtaLexicon.h Common/DtaConstants.h \
	Common/DtaAnnotatedDump.cpp Common/DtaCommand.cpp \
	Common/DtaAnnotatedDump.h Common/DtaCommand.h \
	Common/DtaDev.cpp Common/DtaDevEnterprise.cpp \
	Common/DtaDev.h Common/DtaDevEnterprise.h \
	Common/DtaDevGeneric.cpp Common/DtaDevOpal1.cpp \
	Common/DtaDevGeneric.h Common/DtaDevOpal1.h \
	Common/DtaDevOpal2.cpp Common/DtaDevOpal.cpp \
	Common/DtaDevOpal2.h Common/DtaDevOpal.h \
	Common/DtaDiskType.cpp Common/DtaHashPwd.cpp \
	Common/DtaDiskType.h Common/DtaHashPwd.h \
	Common/DtaHexDump.cpp Common/DtaResponse.cpp \
	Common/DtaHexDump.h Common/DtaResponse.h \
	Common/DtaSession.cpp Common/pbkdf2/blockwise.c \
	Common/DtaSession.h Common/pbkdf2/blockwise.h \
	Common/pbkdf2/chash.c Common/pbkdf2/hmac.c \
	Common/pbkdf2/chash.h Common/pbkdf2/hmac.h \
	Common/pbkdf2/pbkdf2.c Common/pbkdf2/sha1.c \
	Common/pbkdf2/pbkdf2.h Common/pbkdf2/sha1.h \
	Common/pbkdf2/bitops.h Common/pbkdf2/blockwise.h \
	Common/pbkdf2/handy.h Common/pbkdf2/tassert.h 
SEDUTIL_LINUX_CODE = \
	linux/Version.h linux/os.h linux/DtaDevLinuxDrive.h \
	linux/DtaDevLinuxNvme.cpp linux/DtaDevLinuxSata.cpp \
	linux/DtaDevLinuxNvme.h linux/DtaDevLinuxSata.h \
	linux/DtaDevOS.cpp linux/DtaDevOS.h \
	LinuxPBA/GetPassPhrase.cpp LinuxPBA/GetPassPhrase.h
sbin_PROGRAMS = sedutil-cli linuxpba
sedutil_cli_SOURCES = Common/sedutil.cpp Common/DtaOptions.cpp \
	Common/DtaOptions.h \
	$(SEDUTIL_LINUX_CODE) \
	$(SEDUTIL_COMMON_CODE)
CLEANFILES = linux/Version.h
BUILT_SOURCES = linux/Version.h
#
linuxpba_SOURCES = LinuxPBA/LinuxPBA.cpp LinuxPBA/UnlockSEDs.cpp LinuxPBA/UnlockSEDs.h \
	$(SEDUTIL_LINUX_CODE) \
	$(SEDUTIL_COMMON_CODE)
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
