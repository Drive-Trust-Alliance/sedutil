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
	${OBJECTDIR}/_ext/7a2a93ab/MsedAnnotatedDump.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedCommand.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedDev.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedDevEnterprise.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedDevGeneric.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal1.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal2.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedHashPwd.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedHexDump.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedOptions.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedResponse.o \
	${OBJECTDIR}/_ext/7a2a93ab/MsedSession.o \
	${OBJECTDIR}/_ext/7a2a93ab/msed.o \
	${OBJECTDIR}/_ext/cdbabf29/gc-gnulib.o \
	${OBJECTDIR}/_ext/cdbabf29/gc-pbkdf2-sha1.o \
	${OBJECTDIR}/_ext/cdbabf29/hmac-sha1.o \
	${OBJECTDIR}/_ext/cdbabf29/memxor.o \
	${OBJECTDIR}/_ext/cdbabf29/sha1.o \
	${OBJECTDIR}/_ext/5c0/MsedDevLinuxNvme.o \
	${OBJECTDIR}/_ext/5c0/MsedDevLinuxSata.o \
	${OBJECTDIR}/_ext/5c0/MsedDevOS.o


# C Compiler Flags
CFLAGS=-m64 -Wall

# CC Compiler Flags
CCFLAGS=-m64 -Wall
CXXFLAGS=-m64 -Wall

# Fortran Compiler Flags
FFLAGS=-Wall

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sedutil-cli

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sedutil-cli: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sedutil-cli ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/7a2a93ab/MsedAnnotatedDump.o: ../../Common/MsedAnnotatedDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedAnnotatedDump.o ../../Common/MsedAnnotatedDump.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedCommand.o: ../../Common/MsedCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedCommand.o ../../Common/MsedCommand.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedDev.o: ../../Common/MsedDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedDev.o ../../Common/MsedDev.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedDevEnterprise.o: ../../Common/MsedDevEnterprise.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedDevEnterprise.o ../../Common/MsedDevEnterprise.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedDevGeneric.o: ../../Common/MsedDevGeneric.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedDevGeneric.o ../../Common/MsedDevGeneric.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal.o: ../../Common/MsedDevOpal.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal.o ../../Common/MsedDevOpal.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal1.o: ../../Common/MsedDevOpal1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal1.o ../../Common/MsedDevOpal1.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal2.o: ../../Common/MsedDevOpal2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedDevOpal2.o ../../Common/MsedDevOpal2.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedHashPwd.o: ../../Common/MsedHashPwd.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedHashPwd.o ../../Common/MsedHashPwd.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedHexDump.o: ../../Common/MsedHexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedHexDump.o ../../Common/MsedHexDump.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedOptions.o: ../../Common/MsedOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedOptions.o ../../Common/MsedOptions.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedResponse.o: ../../Common/MsedResponse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedResponse.o ../../Common/MsedResponse.cpp

${OBJECTDIR}/_ext/7a2a93ab/MsedSession.o: ../../Common/MsedSession.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/MsedSession.o ../../Common/MsedSession.cpp

${OBJECTDIR}/_ext/7a2a93ab/msed.o: ../../Common/msed.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/msed.o ../../Common/msed.cpp

${OBJECTDIR}/_ext/cdbabf29/gc-gnulib.o: ../../Common/pbdkf2/gc-gnulib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbabf29
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbabf29/gc-gnulib.o ../../Common/pbdkf2/gc-gnulib.c

${OBJECTDIR}/_ext/cdbabf29/gc-pbkdf2-sha1.o: ../../Common/pbdkf2/gc-pbkdf2-sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbabf29
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbabf29/gc-pbkdf2-sha1.o ../../Common/pbdkf2/gc-pbkdf2-sha1.c

${OBJECTDIR}/_ext/cdbabf29/hmac-sha1.o: ../../Common/pbdkf2/hmac-sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbabf29
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbabf29/hmac-sha1.o ../../Common/pbdkf2/hmac-sha1.c

${OBJECTDIR}/_ext/cdbabf29/memxor.o: ../../Common/pbdkf2/memxor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbabf29
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbabf29/memxor.o ../../Common/pbdkf2/memxor.c

${OBJECTDIR}/_ext/cdbabf29/sha1.o: ../../Common/pbdkf2/sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbabf29
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbabf29/sha1.o ../../Common/pbdkf2/sha1.c

${OBJECTDIR}/_ext/5c0/MsedDevLinuxNvme.o: ../MsedDevLinuxNvme.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/MsedDevLinuxNvme.o ../MsedDevLinuxNvme.cpp

${OBJECTDIR}/_ext/5c0/MsedDevLinuxSata.o: ../MsedDevLinuxSata.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/MsedDevLinuxSata.o ../MsedDevLinuxSata.cpp

${OBJECTDIR}/_ext/5c0/MsedDevOS.o: ../MsedDevOS.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/MsedDevOS.o ../MsedDevOS.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sedutil-cli

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
