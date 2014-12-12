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
CND_CONF=Debug_x86_64
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/msed/Class_Skeleton.o \
	${OBJECTDIR}/msed/MsedBaseDev.o \
	${OBJECTDIR}/msed/MsedCommand.o \
	${OBJECTDIR}/msed/MsedHashPwd.o \
	${OBJECTDIR}/msed/MsedHexDump.o \
	${OBJECTDIR}/msed/MsedOptions.o \
	${OBJECTDIR}/msed/MsedResponse.o \
	${OBJECTDIR}/msed/MsedSession.o \
	${OBJECTDIR}/msed/MsedTasks.o \
	${OBJECTDIR}/msed/linux/MsedDev.o \
	${OBJECTDIR}/msed/msed.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Lcryptopp/dist/x86_64 -lcryptopp

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/msed

${CND_DISTDIR}/${CND_CONF}/msed: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/msed ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/msed/Class_Skeleton.o: msed/Class_Skeleton.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/Class_Skeleton.o msed/Class_Skeleton.cpp

${OBJECTDIR}/msed/MsedBaseDev.o: msed/MsedBaseDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedBaseDev.o msed/MsedBaseDev.cpp

${OBJECTDIR}/msed/MsedCommand.o: msed/MsedCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedCommand.o msed/MsedCommand.cpp

${OBJECTDIR}/msed/MsedHashPwd.o: msed/MsedHashPwd.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedHashPwd.o msed/MsedHashPwd.cpp

${OBJECTDIR}/msed/MsedHexDump.o: msed/MsedHexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedHexDump.o msed/MsedHexDump.cpp

${OBJECTDIR}/msed/MsedOptions.o: msed/MsedOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedOptions.o msed/MsedOptions.cpp

${OBJECTDIR}/msed/MsedResponse.o: msed/MsedResponse.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedResponse.o msed/MsedResponse.cpp

${OBJECTDIR}/msed/MsedSession.o: msed/MsedSession.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedSession.o msed/MsedSession.cpp

${OBJECTDIR}/msed/MsedTasks.o: msed/MsedTasks.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/MsedTasks.o msed/MsedTasks.cpp

${OBJECTDIR}/msed/linux/MsedDev.o: msed/linux/MsedDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed/linux
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/linux/MsedDev.o msed/linux/MsedDev.cpp

${OBJECTDIR}/msed/msed.o: msed/msed.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/msed.o msed/msed.cpp

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
