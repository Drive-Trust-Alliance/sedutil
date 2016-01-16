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
CND_CONF=Release_i686
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/msed/MsedAnnotatedDump.o \
	${OBJECTDIR}/msed/MsedCommand.o \
	${OBJECTDIR}/msed/MsedDev.o \
	${OBJECTDIR}/msed/MsedDevEnterprise.o \
	${OBJECTDIR}/msed/MsedDevGeneric.o \
	${OBJECTDIR}/msed/MsedDevOpal.o \
	${OBJECTDIR}/msed/MsedDevOpal1.o \
	${OBJECTDIR}/msed/MsedDevOpal2.o \
	${OBJECTDIR}/msed/MsedHashPwd.o \
	${OBJECTDIR}/msed/MsedHexDump.o \
	${OBJECTDIR}/msed/MsedOptions.o \
	${OBJECTDIR}/msed/MsedResponse.o \
	${OBJECTDIR}/msed/MsedSession.o \
	${OBJECTDIR}/msed/linux/MsedDevOS.o \
	${OBJECTDIR}/msed/msed.o \
	${OBJECTDIR}/msed/pbdkf2/gc-gnulib.o \
	${OBJECTDIR}/msed/pbdkf2/gc-pbkdf2-sha1.o \
	${OBJECTDIR}/msed/pbdkf2/hmac-sha1.o \
	${OBJECTDIR}/msed/pbdkf2/memxor.o \
	${OBJECTDIR}/msed/pbdkf2/sha1.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32 -Wall
CXXFLAGS=-m32 -Wall

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/msed

${CND_DISTDIR}/${CND_CONF}/msed: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/msed ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/msed/MsedAnnotatedDump.o: msed/MsedAnnotatedDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedAnnotatedDump.o msed/MsedAnnotatedDump.cpp

${OBJECTDIR}/msed/MsedCommand.o: msed/MsedCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedCommand.o msed/MsedCommand.cpp

${OBJECTDIR}/msed/MsedDev.o: msed/MsedDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedDev.o msed/MsedDev.cpp

${OBJECTDIR}/msed/MsedDevEnterprise.o: msed/MsedDevEnterprise.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedDevEnterprise.o msed/MsedDevEnterprise.cpp

${OBJECTDIR}/msed/MsedDevGeneric.o: msed/MsedDevGeneric.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedDevGeneric.o msed/MsedDevGeneric.cpp

${OBJECTDIR}/msed/MsedDevOpal.o: msed/MsedDevOpal.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedDevOpal.o msed/MsedDevOpal.cpp

${OBJECTDIR}/msed/MsedDevOpal1.o: msed/MsedDevOpal1.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedDevOpal1.o msed/MsedDevOpal1.cpp

${OBJECTDIR}/msed/MsedDevOpal2.o: msed/MsedDevOpal2.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedDevOpal2.o msed/MsedDevOpal2.cpp

${OBJECTDIR}/msed/MsedHashPwd.o: msed/MsedHashPwd.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedHashPwd.o msed/MsedHashPwd.cpp

${OBJECTDIR}/msed/MsedHexDump.o: msed/MsedHexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedHexDump.o msed/MsedHexDump.cpp

${OBJECTDIR}/msed/MsedOptions.o: msed/MsedOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedOptions.o msed/MsedOptions.cpp

${OBJECTDIR}/msed/MsedResponse.o: msed/MsedResponse.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedResponse.o msed/MsedResponse.cpp

${OBJECTDIR}/msed/MsedSession.o: msed/MsedSession.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedSession.o msed/MsedSession.cpp

${OBJECTDIR}/msed/linux/MsedDevOS.o: msed/linux/MsedDevOS.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed/linux
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/linux/MsedDevOS.o msed/linux/MsedDevOS.cpp

${OBJECTDIR}/msed/msed.o: msed/msed.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -s -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/msed.o msed/msed.cpp

${OBJECTDIR}/msed/pbdkf2/gc-gnulib.o: msed/pbdkf2/gc-gnulib.c 
	${MKDIR} -p ${OBJECTDIR}/msed/pbdkf2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/pbdkf2/gc-gnulib.o msed/pbdkf2/gc-gnulib.c

${OBJECTDIR}/msed/pbdkf2/gc-pbkdf2-sha1.o: msed/pbdkf2/gc-pbkdf2-sha1.c 
	${MKDIR} -p ${OBJECTDIR}/msed/pbdkf2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/pbdkf2/gc-pbkdf2-sha1.o msed/pbdkf2/gc-pbkdf2-sha1.c

${OBJECTDIR}/msed/pbdkf2/hmac-sha1.o: msed/pbdkf2/hmac-sha1.c 
	${MKDIR} -p ${OBJECTDIR}/msed/pbdkf2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/pbdkf2/hmac-sha1.o msed/pbdkf2/hmac-sha1.c

${OBJECTDIR}/msed/pbdkf2/memxor.o: msed/pbdkf2/memxor.c 
	${MKDIR} -p ${OBJECTDIR}/msed/pbdkf2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/pbdkf2/memxor.o msed/pbdkf2/memxor.c

${OBJECTDIR}/msed/pbdkf2/sha1.o: msed/pbdkf2/sha1.c 
	${MKDIR} -p ${OBJECTDIR}/msed/pbdkf2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/pbdkf2/sha1.o msed/pbdkf2/sha1.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/msed

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
