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
include LinuxPBA-Makefile.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/760796236/MsedCommand.o \
	${OBJECTDIR}/_ext/760796236/MsedDev.o \
	${OBJECTDIR}/_ext/760796236/MsedDevGeneric.o \
	${OBJECTDIR}/_ext/760796236/MsedDevOpal.o \
	${OBJECTDIR}/_ext/760796236/MsedDevOpal1.o \
	${OBJECTDIR}/_ext/760796236/MsedDevOpal2.o \
	${OBJECTDIR}/_ext/760796236/MsedHashPwd.o \
	${OBJECTDIR}/_ext/760796236/MsedHexDump.o \
	${OBJECTDIR}/_ext/760796236/MsedAnnotatedDump.o \
	${OBJECTDIR}/_ext/760796236/MsedResponse.o \
	${OBJECTDIR}/_ext/760796236/MsedSession.o \
	${OBJECTDIR}/_ext/764652249/MsedDevOS.o \
	${OBJECTDIR}/_ext/1957840128/gc-gnulib.o \
	${OBJECTDIR}/_ext/1957840128/gc-pbkdf2-sha1.o \
	${OBJECTDIR}/_ext/1957840128/hmac-sha1.o \
	${OBJECTDIR}/_ext/1957840128/memxor.o \
	${OBJECTDIR}/_ext/1957840128/sha1.o \
	${OBJECTDIR}/GetPassPhrase.o \
	${OBJECTDIR}/LinuxPBA.o \
	${OBJECTDIR}/UnlockSEDs.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64 -DMSEDDEBUG
CXXFLAGS=-m64 -DMSEDDEBUG

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

${OBJECTDIR}/_ext/760796236/MsedCommand.o: ../msed/MsedCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedCommand.o ../msed/MsedCommand.cpp

${OBJECTDIR}/_ext/760796236/MsedDev.o: ../msed/MsedDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedDev.o ../msed/MsedDev.cpp

${OBJECTDIR}/_ext/760796236/MsedDevGeneric.o: ../msed/MsedDevGeneric.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedDevGeneric.o ../msed/MsedDevGeneric.cpp

${OBJECTDIR}/_ext/760796236/MsedDevOpal.o: ../msed/MsedDevOpal.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedDevOpal.o ../msed/MsedDevOpal.cpp

${OBJECTDIR}/_ext/760796236/MsedDevOpal1.o: ../msed/MsedDevOpal1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedDevOpal1.o ../msed/MsedDevOpal1.cpp

${OBJECTDIR}/_ext/760796236/MsedDevOpal2.o: ../msed/MsedDevOpal2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedDevOpal2.o ../msed/MsedDevOpal2.cpp

${OBJECTDIR}/_ext/760796236/MsedHashPwd.o: ../msed/MsedHashPwd.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedHashPwd.o ../msed/MsedHashPwd.cpp

${OBJECTDIR}/_ext/760796236/MsedHexDump.o: ../msed/MsedHexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedHexDump.o ../msed/MsedHexDump.cpp

${OBJECTDIR}/_ext/760796236/MsedAnnotatedDump.o: ../msed/MsedAnnotatedDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedAnnotatedDump.o ../msed/MsedAnnotatedDump.cpp

${OBJECTDIR}/_ext/760796236/MsedResponse.o: ../msed/MsedResponse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedResponse.o ../msed/MsedResponse.cpp

${OBJECTDIR}/_ext/760796236/MsedSession.o: ../msed/MsedSession.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/760796236
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/760796236/MsedSession.o ../msed/MsedSession.cpp

${OBJECTDIR}/_ext/764652249/MsedDevOS.o: ../msed/linux/MsedDevOS.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/764652249
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/764652249/MsedDevOS.o ../msed/linux/MsedDevOS.cpp

${OBJECTDIR}/_ext/1957840128/gc-gnulib.o: ../msed/pbdkf2/gc-gnulib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1957840128
	${RM} "$@.d"
	$(COMPILE.c) -g -s -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1957840128/gc-gnulib.o ../msed/pbdkf2/gc-gnulib.c

${OBJECTDIR}/_ext/1957840128/gc-pbkdf2-sha1.o: ../msed/pbdkf2/gc-pbkdf2-sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1957840128
	${RM} "$@.d"
	$(COMPILE.c) -g -s -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1957840128/gc-pbkdf2-sha1.o ../msed/pbdkf2/gc-pbkdf2-sha1.c

${OBJECTDIR}/_ext/1957840128/hmac-sha1.o: ../msed/pbdkf2/hmac-sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1957840128
	${RM} "$@.d"
	$(COMPILE.c) -g -s -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1957840128/hmac-sha1.o ../msed/pbdkf2/hmac-sha1.c

${OBJECTDIR}/_ext/1957840128/memxor.o: ../msed/pbdkf2/memxor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1957840128
	${RM} "$@.d"
	$(COMPILE.c) -g -s -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1957840128/memxor.o ../msed/pbdkf2/memxor.c

${OBJECTDIR}/_ext/1957840128/sha1.o: ../msed/pbdkf2/sha1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1957840128
	${RM} "$@.d"
	$(COMPILE.c) -g -s -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1957840128/sha1.o ../msed/pbdkf2/sha1.c

${OBJECTDIR}/GetPassPhrase.o: GetPassPhrase.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GetPassPhrase.o GetPassPhrase.cpp

${OBJECTDIR}/LinuxPBA.o: LinuxPBA.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LinuxPBA.o LinuxPBA.cpp

${OBJECTDIR}/UnlockSEDs.o: UnlockSEDs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../msed/linux -I../msed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UnlockSEDs.o UnlockSEDs.cpp

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
