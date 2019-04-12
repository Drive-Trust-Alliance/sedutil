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
	${OBJECTDIR}/_ext/7a2a93ab/DtaAnnotatedDump.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaCommand.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDev.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDevEnterprise.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDevGeneric.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal1.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal2.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpalite.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDevPyrite1.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaDevPyrite2.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaHashPwd.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaHexDump.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaOptions.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaResponse.o \
	${OBJECTDIR}/_ext/7a2a93ab/DtaSession.o \
	${OBJECTDIR}/_ext/cdbdd37b/blockwise.o \
	${OBJECTDIR}/_ext/cdbdd37b/chash.o \
	${OBJECTDIR}/_ext/cdbdd37b/hmac.o \
	${OBJECTDIR}/_ext/cdbdd37b/pbkdf2.o \
	${OBJECTDIR}/_ext/cdbdd37b/sha1.o \
	${OBJECTDIR}/_ext/7a2a93ab/sedutil.o \
	${OBJECTDIR}/_ext/5c0/DtaDevLinuxNvme.o \
	${OBJECTDIR}/_ext/5c0/DtaDevLinuxSata.o \
	${OBJECTDIR}/_ext/5c0/DtaDevOS.o


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

${OBJECTDIR}/_ext/7a2a93ab/DtaAnnotatedDump.o: ../../Common/DtaAnnotatedDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaAnnotatedDump.o ../../Common/DtaAnnotatedDump.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaCommand.o: ../../Common/DtaCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaCommand.o ../../Common/DtaCommand.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDev.o: ../../Common/DtaDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDev.o ../../Common/DtaDev.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDevEnterprise.o: ../../Common/DtaDevEnterprise.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDevEnterprise.o ../../Common/DtaDevEnterprise.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDevGeneric.o: ../../Common/DtaDevGeneric.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDevGeneric.o ../../Common/DtaDevGeneric.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal.o: ../../Common/DtaDevOpal.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal.o ../../Common/DtaDevOpal.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal1.o: ../../Common/DtaDevOpal1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal1.o ../../Common/DtaDevOpal1.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal2.o: ../../Common/DtaDevOpal2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpal2.o ../../Common/DtaDevOpal2.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpalite.o: ../../Common/DtaDevOpalite.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDevOpalite.o ../../Common/DtaDevOpalite.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDevPyrite1.o: ../../Common/DtaDevPyrite1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDevPyrite1.o ../../Common/DtaDevPyrite1.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaDevPyrite2.o: ../../Common/DtaDevPyrite2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaDevPyrite2.o ../../Common/DtaDevPyrite2.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaHashPwd.o: ../../Common/DtaHashPwd.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaHashPwd.o ../../Common/DtaHashPwd.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaHexDump.o: ../../Common/DtaHexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaHexDump.o ../../Common/DtaHexDump.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaOptions.o: ../../Common/DtaOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaOptions.o ../../Common/DtaOptions.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaResponse.o: ../../Common/DtaResponse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaResponse.o ../../Common/DtaResponse.cpp

${OBJECTDIR}/_ext/7a2a93ab/DtaSession.o: ../../Common/DtaSession.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/DtaSession.o ../../Common/DtaSession.cpp

${OBJECTDIR}/_ext/cdbdd37b/blockwise.o: ../../Common/pbkdf2/blockwise.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbdd37b
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbdd37b/blockwise.o ../../Common/pbkdf2/blockwise.c

${OBJECTDIR}/_ext/cdbdd37b/chash.o: ../../Common/pbkdf2/chash.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbdd37b
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbdd37b/chash.o ../../Common/pbkdf2/chash.c

${OBJECTDIR}/_ext/cdbdd37b/hmac.o: ../../Common/pbkdf2/hmac.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbdd37b
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbdd37b/hmac.o ../../Common/pbkdf2/hmac.c

${OBJECTDIR}/_ext/cdbdd37b/pbkdf2.o: ../../Common/pbkdf2/pbkdf2.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbdd37b
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbdd37b/pbkdf2.o ../../Common/pbkdf2/pbkdf2.c

${OBJECTDIR}/_ext/cdbdd37b/sha1.o: ../../Common/pbkdf2/sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/cdbdd37b
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cdbdd37b/sha1.o ../../Common/pbkdf2/sha1.c

${OBJECTDIR}/_ext/7a2a93ab/sedutil.o: ../../Common/sedutil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2a93ab
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2a93ab/sedutil.o ../../Common/sedutil.cpp

${OBJECTDIR}/_ext/5c0/DtaDevLinuxNvme.o: ../DtaDevLinuxNvme.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/DtaDevLinuxNvme.o ../DtaDevLinuxNvme.cpp

${OBJECTDIR}/_ext/5c0/DtaDevLinuxSata.o: ../DtaDevLinuxSata.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/DtaDevLinuxSata.o ../DtaDevLinuxSata.cpp

${OBJECTDIR}/_ext/5c0/DtaDevOS.o: ../DtaDevOS.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.cc) -Werror -I.. -I../../Common -I../../Common/pbkdf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/DtaDevOS.o ../DtaDevOS.cpp

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
