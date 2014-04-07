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
	${OBJECTDIR}/msed/TCGbaseDev.o \
	${OBJECTDIR}/msed/TCGcommand.o \
	${OBJECTDIR}/msed/TCGsession.o \
	${OBJECTDIR}/msed/TCGtasks.o \
	${OBJECTDIR}/msed/diskList.o \
	${OBJECTDIR}/msed/hexDump.o \
	${OBJECTDIR}/msed/linux/TCGdev.o \
	${OBJECTDIR}/msed/msed.o \
	${OBJECTDIR}/msed/options.o


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
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/Class_Skeleton.o msed/Class_Skeleton.cpp

${OBJECTDIR}/msed/TCGbaseDev.o: msed/TCGbaseDev.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/TCGbaseDev.o msed/TCGbaseDev.cpp

${OBJECTDIR}/msed/TCGcommand.o: msed/TCGcommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/TCGcommand.o msed/TCGcommand.cpp

${OBJECTDIR}/msed/TCGsession.o: msed/TCGsession.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/TCGsession.o msed/TCGsession.cpp

${OBJECTDIR}/msed/TCGtasks.o: msed/TCGtasks.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/TCGtasks.o msed/TCGtasks.cpp

${OBJECTDIR}/msed/diskList.o: msed/diskList.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/diskList.o msed/diskList.cpp

${OBJECTDIR}/msed/hexDump.o: msed/hexDump.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/hexDump.o msed/hexDump.cpp

${OBJECTDIR}/msed/linux/TCGdev.o: msed/linux/TCGdev.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed/linux
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/linux/TCGdev.o msed/linux/TCGdev.cpp

${OBJECTDIR}/msed/msed.o: msed/msed.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/msed.o msed/msed.cpp

${OBJECTDIR}/msed/options.o: msed/options.cpp 
	${MKDIR} -p ${OBJECTDIR}/msed
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Imsed/linux -Imsed -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msed/options.o msed/options.cpp

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
