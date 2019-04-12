#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release_x86_64
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/7daaf93a/DtaCommand.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaDev.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaDevGeneric.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal1.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal2.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaHashPwd.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaHexDump.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaResponse.o \
	${OBJECTDIR}/_ext/7daaf93a/DtaSession.o \
	${OBJECTDIR}/_ext/b7b9df0c/blockwise.o \
	${OBJECTDIR}/_ext/b7b9df0c/chash.o \
	${OBJECTDIR}/_ext/b7b9df0c/hmac.o \
	${OBJECTDIR}/_ext/b7b9df0c/pbkdf2.o \
	${OBJECTDIR}/_ext/b7b9df0c/sha1.o \
	${OBJECTDIR}/_ext/822bcbe5/DtaDevLinuxNvme.o \
	${OBJECTDIR}/_ext/822bcbe5/DtaDevLinuxSata.o \
	${OBJECTDIR}/_ext/822bcbe5/DtaDevOS.o \
	${OBJECTDIR}/GetPassPhrase.o \
	${OBJECTDIR}/LinuxPBA.o \
	${OBJECTDIR}/UnlockSEDs.o


# C Compiler Flags
CFLAGS=-m64 -Wall

# CC Compiler Flags
CCFLAGS=-m64 -Wall
CXXFLAGS=-m64 -Wall

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linuxpba

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linuxpba: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linuxpba ${OBJECTFILES} ${LDLIBSOPTIONS} -s

${OBJECTDIR}/_ext/7daaf93a/DtaCommand.o: ../Common/DtaCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaCommand.o ../Common/DtaCommand.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaDev.o: ../Common/DtaDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaDev.o ../Common/DtaDev.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaDevGeneric.o: ../Common/DtaDevGeneric.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaDevGeneric.o ../Common/DtaDevGeneric.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal.o: ../Common/DtaDevOpal.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal.o ../Common/DtaDevOpal.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal1.o: ../Common/DtaDevOpal1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal1.o ../Common/DtaDevOpal1.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal2.o: ../Common/DtaDevOpal2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaDevOpal2.o ../Common/DtaDevOpal2.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaDevOpalite.o: ../Common/DtaDevOpalite.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaDevOpalite.o ../Common/DtaDevOpalite.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaDevPyrite1.o: ../Common/DtaDevPyrite1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaDevPyrite1.o ../Common/DtaDevPyrite1.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaDevPyrite2.o: ../Common/DtaDevPyrite2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaDevPyrite2.o ../Common/DtaDevPyrite2.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaHashPwd.o: ../Common/DtaHashPwd.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaHashPwd.o ../Common/DtaHashPwd.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaHexDump.o: ../Common/DtaHexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaHexDump.o ../Common/DtaHexDump.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaResponse.o: ../Common/DtaResponse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaResponse.o ../Common/DtaResponse.cpp

${OBJECTDIR}/_ext/7daaf93a/DtaSession.o: ../Common/DtaSession.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/DtaSession.o ../Common/DtaSession.cpp

${OBJECTDIR}/_ext/b7b9df0c/blockwise.o: ../Common/pbkdf2/blockwise.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b7b9df0c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b7b9df0c/blockwise.o ../Common/pbkdf2/blockwise.c

${OBJECTDIR}/_ext/b7b9df0c/chash.o: ../Common/pbkdf2/chash.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b7b9df0c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b7b9df0c/chash.o ../Common/pbkdf2/chash.c

${OBJECTDIR}/_ext/b7b9df0c/hmac.o: ../Common/pbkdf2/hmac.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b7b9df0c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b7b9df0c/hmac.o ../Common/pbkdf2/hmac.c

${OBJECTDIR}/_ext/b7b9df0c/pbkdf2.o: ../Common/pbkdf2/pbkdf2.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b7b9df0c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b7b9df0c/pbkdf2.o ../Common/pbkdf2/pbkdf2.c

${OBJECTDIR}/_ext/b7b9df0c/sha1.o: ../Common/pbkdf2/sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b7b9df0c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b7b9df0c/sha1.o ../Common/pbkdf2/sha1.c

${OBJECTDIR}/_ext/822bcbe5/DtaDevLinuxNvme.o: ../linux/DtaDevLinuxNvme.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/822bcbe5
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/822bcbe5/DtaDevLinuxNvme.o ../linux/DtaDevLinuxNvme.cpp

${OBJECTDIR}/_ext/822bcbe5/DtaDevLinuxSata.o: ../linux/DtaDevLinuxSata.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/822bcbe5
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/822bcbe5/DtaDevLinuxSata.o ../linux/DtaDevLinuxSata.cpp

${OBJECTDIR}/_ext/822bcbe5/DtaDevOS.o: ../linux/DtaDevOS.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/822bcbe5
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/822bcbe5/DtaDevOS.o ../linux/DtaDevOS.cpp

${OBJECTDIR}/GetPassPhrase.o: GetPassPhrase.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GetPassPhrase.o GetPassPhrase.cpp

${OBJECTDIR}/LinuxPBA.o: LinuxPBA.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LinuxPBA.o LinuxPBA.cpp

${OBJECTDIR}/UnlockSEDs.o: UnlockSEDs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -I../linux -I../Common -I../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UnlockSEDs.o UnlockSEDs.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linuxpba

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
