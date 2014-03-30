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
	${OBJECTDIR}/msed/Class_Skeleton.o \
	${OBJECTDIR}/msed/Discovery0.o \
	${OBJECTDIR}/msed/HexDump.o \
	${OBJECTDIR}/msed/TCGCommand.o \
	${OBJECTDIR}/msed/linux/Device_linux.o \
	${OBJECTDIR}/msed/linux/DiskList_linux.o \
	${OBJECTDIR}/msed/msed.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/msed

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/msed: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/msed ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/msed/Class_Skeleton.o: msed/Class_Skeleton.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/Class_Skeleton.o msed/Class_Skeleton.cpp

${OBJECTDIR}/msed/Discovery0.o: msed/Discovery0.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/Discovery0.o msed/Discovery0.cpp

${OBJECTDIR}/msed/HexDump.o: msed/HexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/HexDump.o msed/HexDump.cpp

${OBJECTDIR}/msed/TCGCommand.o: msed/TCGCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/TCGCommand.o msed/TCGCommand.cpp

${OBJECTDIR}/msed/linux/Device_linux.o: msed/linux/Device_linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed/linux
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/linux/Device_linux.o msed/linux/Device_linux.cpp

${OBJECTDIR}/msed/linux/DiskList_linux.o: msed/linux/DiskList_linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed/linux
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/linux/DiskList_linux.o msed/linux/DiskList_linux.cpp

${OBJECTDIR}/msed/msed.o: msed/msed.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/msed.o msed/msed.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/msed

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
