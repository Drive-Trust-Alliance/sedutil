AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CFLAGS = -Wall -Werror -std=c11
AM_CXXFLAGS = -Wall -Werror -std=c++11 -I./Common -I./Common/pbkdf2 -I./linux -I./LinuxPBA
SEDUTIL_COMMON_CODE = \
	Common/DtaAnnotatedDump.cpp Common/DtaAnnotatedDump.h \
	Common/DtaCommand.cpp Common/DtaCommand.h \
	Common/DtaDev.cpp Common/DtaDev.h \
	Common/DtaDevEnterprise.cpp Common/DtaDevEnterprise.h \
	Common/DtaDevGeneric.cpp Common/DtaDevGeneric.h \
	Common/DtaDevOpal1.cpp Common/DtaDevOpal1.h \
	Common/DtaDevOpal2.cpp Common/DtaDevOpal2.h \
	Common/DtaDevOpal.cpp Common/DtaDevOpal.h \
	Common/DtaDiskType.cpp Common/DtaDiskType.h \
	Common/DtaHashPwd.cpp Common/DtaHashPwd.h \
	Common/DtaHexDump.cpp Common/DtaHexDump.h \
	Common/DtaResponse.cpp Common/DtaResponse.h \
	Common/DtaSession.cpp Common/DtaSession.h \
	Common/DtaStructures.h Common/log.h Common/DtaLexicon.h Common/DtaConstants.h Common/DtaEndianFixup.h \
	Common/pbkdf2/blockwise.c Common/pbkdf2/blockwise.h Common/pbkdf2/chash.c Common/pbkdf2/chash.h Common/pbkdf2/handy.h \
	Common/pbkdf2/hmac.c Common/pbkdf2/hmac.h Common/pbkdf2/pbkdf2.c Common/pbkdf2/pbkdf2.h Common/pbkdf2/sha1.c \
	Common/pbkdf2/sha1.h Common/pbkdf2/tassert.h Common/pbkdf2/bitops.h
sbin_PROGRAMS = sedutil-cli linuxpba
sedutil_cli_SOURCES = linux/Version.h Common/sedutil.cpp Common/DtaOptions.cpp Common/DtaOptions.h \
	\
	linux/DtaDevLinuxNvme.cpp linux/DtaDevLinuxNvme.h \
	linux/DtaDevLinuxSata.cpp linux/DtaDevLinuxSata.h \
	linux/DtaDevOS.cpp linux/DtaDevOS.h \
	linux/DtaDevLinuxDrive.h linux/os.h \
    LinuxPBA/GetPassPhrase.cpp \
	$(SEDUTIL_COMMON_CODE)
CLEANFILES = linux/Version.h
BUILT_SOURCES = linux/Version.h
#
linuxpba_SOURCES = LinuxPBA/LinuxPBA.cpp LinuxPBA/GetPassPhrase.cpp LinuxPBA/UnlockSEDs.cpp \
	 LinuxPBA/GetPassPhrase.h LinuxPBA/UnlockSEDs.h \
	\
	linux/DtaDevLinuxNvme.cpp linux/DtaDevLinuxNvme.h \
	linux/DtaDevLinuxSata.cpp linux/DtaDevLinuxSata.h \
	linux/DtaDevOS.cpp linux/DtaDevOS.h \
	linux/DtaDevLinuxDrive.h linux/os.h \
	\
	$(SEDUTIL_COMMON_CODE)
EXTRA_DIST = linux/GitVersion.sh linux/PSIDRevert_LINUX.txt linux/TestSuite.sh README.md docs/sedutil-cli.8
man_MANS = docs/sedutil-cli.8
linux/Version.h:
	./linux/GitVersion.sh > linux/Version.h
.INTERMEDIATE: linux/Version.h

