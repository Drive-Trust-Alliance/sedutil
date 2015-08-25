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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/2108356922/MsedCommand.o \
	${OBJECTDIR}/_ext/2108356922/MsedDev.o \
	${OBJECTDIR}/_ext/2108356922/MsedDevGeneric.o \
	${OBJECTDIR}/_ext/2108356922/MsedDevOpal.o \
	${OBJECTDIR}/_ext/2108356922/MsedDevOpal1.o \
	${OBJECTDIR}/_ext/2108356922/MsedDevOpal2.o \
	${OBJECTDIR}/_ext/2108356922/MsedHashPwd.o \
	${OBJECTDIR}/_ext/2108356922/MsedHexDump.o \
	${OBJECTDIR}/_ext/2108356922/MsedResponse.o \
	${OBJECTDIR}/_ext/2108356922/MsedSession.o \
	${OBJECTDIR}/_ext/1212757318/gc-gnulib.o \
	${OBJECTDIR}/_ext/1212757318/gc-pbkdf2-sha1.o \
	${OBJECTDIR}/_ext/1212757318/hmac-sha1.o \
	${OBJECTDIR}/_ext/1212757318/memxor.o \
	${OBJECTDIR}/_ext/1212757318/sha1.o \
	${OBJECTDIR}/_ext/2111058971/MsedDevOS.o \
	${OBJECTDIR}/GetPassPhrase.o \
	${OBJECTDIR}/LinuxPBA.o \
	${OBJECTDIR}/UnlockSEDs.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lcurses

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linuxpba

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linuxpba: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linuxpba ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/2108356922/MsedCommand.o: ../Common/MsedCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedCommand.o ../Common/MsedCommand.cpp

${OBJECTDIR}/_ext/2108356922/MsedDev.o: ../Common/MsedDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedDev.o ../Common/MsedDev.cpp

${OBJECTDIR}/_ext/2108356922/MsedDevGeneric.o: ../Common/MsedDevGeneric.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedDevGeneric.o ../Common/MsedDevGeneric.cpp

${OBJECTDIR}/_ext/2108356922/MsedDevOpal.o: ../Common/MsedDevOpal.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedDevOpal.o ../Common/MsedDevOpal.cpp

${OBJECTDIR}/_ext/2108356922/MsedDevOpal1.o: ../Common/MsedDevOpal1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedDevOpal1.o ../Common/MsedDevOpal1.cpp

${OBJECTDIR}/_ext/2108356922/MsedDevOpal2.o: ../Common/MsedDevOpal2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedDevOpal2.o ../Common/MsedDevOpal2.cpp

${OBJECTDIR}/_ext/2108356922/MsedHashPwd.o: ../Common/MsedHashPwd.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedHashPwd.o ../Common/MsedHashPwd.cpp

${OBJECTDIR}/_ext/2108356922/MsedHexDump.o: ../Common/MsedHexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedHexDump.o ../Common/MsedHexDump.cpp

${OBJECTDIR}/_ext/2108356922/MsedResponse.o: ../Common/MsedResponse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedResponse.o ../Common/MsedResponse.cpp

${OBJECTDIR}/_ext/2108356922/MsedSession.o: ../Common/MsedSession.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2108356922
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2108356922/MsedSession.o ../Common/MsedSession.cpp

${OBJECTDIR}/_ext/1212757318/gc-gnulib.o: ../Common/pbdkf2/gc-gnulib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1212757318
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1212757318/gc-gnulib.o ../Common/pbdkf2/gc-gnulib.c

${OBJECTDIR}/_ext/1212757318/gc-pbkdf2-sha1.o: ../Common/pbdkf2/gc-pbkdf2-sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1212757318
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1212757318/gc-pbkdf2-sha1.o ../Common/pbdkf2/gc-pbkdf2-sha1.c

${OBJECTDIR}/_ext/1212757318/hmac-sha1.o: ../Common/pbdkf2/hmac-sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1212757318
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1212757318/hmac-sha1.o ../Common/pbdkf2/hmac-sha1.c

${OBJECTDIR}/_ext/1212757318/memxor.o: ../Common/pbdkf2/memxor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1212757318
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1212757318/memxor.o ../Common/pbdkf2/memxor.c

${OBJECTDIR}/_ext/1212757318/sha1.o: ../Common/pbdkf2/sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1212757318
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1212757318/sha1.o ../Common/pbdkf2/sha1.c

${OBJECTDIR}/_ext/2111058971/MsedDevOS.o: ../linux/MsedDevOS.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2111058971/MsedDevOS.o ../linux/MsedDevOS.cpp

${OBJECTDIR}/GetPassPhrase.o: GetPassPhrase.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GetPassPhrase.o GetPassPhrase.cpp

${OBJECTDIR}/LinuxPBA.o: LinuxPBA.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LinuxPBA.o LinuxPBA.cpp

${OBJECTDIR}/UnlockSEDs.o: UnlockSEDs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../linux -I../Common -I../Common/pbdkf2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UnlockSEDs.o UnlockSEDs.cpp

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
