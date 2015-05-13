
#Copyright (c) 2013-2014, Swedish National Road and Transport Research Institute
#All rights reserved.

#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#    Neither the name of the Swedish National Road and Transport Research Institute nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


# - Find OculusSDK
# Find the native OculusSDK headers and libraries.
#
#  OCULUS_SDK_INCLUDE_DIRS - where to find OVR.h, etc.
#  OCULUS_SDK_LIBRARIES    - List of libraries when using OculusSDK.
#  OCULUS_SDK_FOUND        - True if OculusSDK found.
#  OCULUS_SDK_VERSION      - Version of the OculusSDK if found

IF (DEFINED ENV{OCULUS_SDK_ROOT_DIR})
	message("Environment variable OCULUS_SDK_ROOT_DIR found. This will be the used as OculusSDK root dir.")
    SET(OCULUS_SDK_ROOT_DIR "$ENV{OCULUS_SDK_ROOT_DIR}")
#ELSEIF(DEFINED ENV{OCULUS_HOME})
#	message("Environment variable OCULUS_HOME found. This will be the used as OculusSDK root dir.")
#    SET(OCULUS_SDK_ROOT_DIR "$ENV{OCULUS_HOME}")
ELSEIF (DEFINED OCULUS_SDK_ROOT_DIR)
	SET(OCULUS_SDK_ROOT_DIR
	    "${OCULUS_SDK_ROOT_DIR}"
	    CACHE
	    PATH
	    "Root directory to search for OculusSDK set to ${OCULUS_SDK_ROOT_DIR}")
ELSE()
	MESSAGE(FATAL_ERROR "No OCULUS_SDK_ROOT_DIR environment variable set or cmake variable OCULUS_SDK_ROOT_DIR set.")
ENDIF()

# Look for the header file.
FIND_PATH(OCULUS_SDK_INCLUDE_DIR NAMES OVR.h HINTS			# useless??
	${OCULUS_SDK_ROOT_DIR}/LibOVR/Include )

FIND_PATH(OCULUS_SDK_CAPI_DIR NAMES OVR_CAPI.h HINTS 		# useless??
	${OCULUS_SDK_ROOT_DIR}/LibOVR/Src )

FIND_PATH(OCULUS_SDK_KERNEL_DIR NAMES OVR_CAPI.h HINTS 		# useless??
	${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Src )

set(OCULUS_SDK_INCLUDE_DIR ${OCULUS_SDK_ROOT_DIR}/LibOVR/Include)
set(OCULUS_SDK_CAPI_DIR ${OCULUS_SDK_ROOT_DIR}/LibOVR/Src)
set(OCULUS_SDK_KERNEL_DIR ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Src)

set( OCULUS_SDK_INCLUDE_DIRS "${OCULUS_SDK_INCLUDE_DIR};${OCULUS_SDK_CAPI_DIR};${OCULUS_SDK_KERNEL_DIR}" )

# Determine architecture
IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
	IF(UNIX)
		SET(_OCULUS_SDK_LIB_ARCH "x86_64")
	ENDIF()
	IF(MSVC)
		SET(_OCULUS_SDK_LIB_ARCH "x64")
	ENDIF()
ELSE()
	IF(UNIX)
		SET(_OCULUS_SDK_LIB_ARCH "i386")
	ENDIF()
	IF(MSVC)
		SET(_OCULUS_SDK_LIB_ARCH "Win32")
	ENDIF()
ENDIF()

MARK_AS_ADVANCED(_OCULUS_SDK_LIB_ARCH)

# Append "d" to debug libs on windows platform
IF (WIN32)
	SET(CMAKE_DEBUG_POSTFIX d)
ENDIF()

# Determine the compiler version for Visual Studio
IF (MSVC)
	# Visual Studio 2010
	IF(MSVC10)
		SET(_OCULUS_MSVC_DIR "VS2010")
	ENDIF()
	# Visual Studio 2012
	IF(MSVC11)
		SET(_OCULUS_MSVC_DIR "VS2012")
	ENDIF()
	# Visual Studio 2013
	IF(MSVC12)
		SET(_OCULUS_MSVC_DIR "VS2013")
	ENDIF()
ENDIF()

# Try to ascertain the version of the SDK
IF(OCULUS_SDK_INCLUDE_DIRS)
	SET(_OCULUS_VERSION_FILE "${OCULUS_SDK_INCLUDE_DIRS}/OVR_Version.h")
	
	IF(EXISTS "${_OCULUS_VERSION_FILE}") 
		FILE(STRINGS "${_OCULUS_VERSION_FILE}" _OCULUS_VERSION_FILE_CONTENTS REGEX "#define OVR_[A-Z]+_VERSION[ \t]+[0-9]+")

		STRING(REGEX REPLACE ".*#define OVR_MAJOR_VERSION[ \t]+([0-9]+).*" "\\1" OCULUS_SDK_VERSION_MAJOR ${_OCULUS_VERSION_FILE_CONTENTS})
		STRING(REGEX REPLACE ".*#define OVR_MINOR_VERSION[ \t]+([0-9]+).*" "\\1" OCULUS_SDK_VERSION_MINOR ${_OCULUS_VERSION_FILE_CONTENTS})
		STRING(REGEX REPLACE ".*#define OVR_BUILD_VERSION[ \t]+([0-9]+).*" "\\1" OCULUS_SDK_VERSION_BUILD ${_OCULUS_VERSION_FILE_CONTENTS})

		SET(OCULUS_SDK_VERSION "${OCULUS_SDK_VERSION_MAJOR}.${OCULUS_SDK_VERSION_MINOR}.${OCULUS_SDK_VERSION_BUILD}" CACHE INTERNAL "The version of Oculus SDK which was detected")
	ENDIF()
ENDIF()

# Locate Oculus license file
SET(_OCULUS_SDK_LICENSE_FILE "${OCULUS_SDK_ROOT_DIR}/LICENSE.txt")
IF(EXISTS "${_OCULUS_SDK_LICENSE_FILE}") 
	SET(OCULUS_SDK_LICENSE_FILE "${_OCULUS_SDK_LICENSE_FILE}" CACHE INTERNAL "The location of the Oculus SDK license file")
ENDIF()

# Look for the library.
FIND_LIBRARY(OCULUS_SDK_LIBRARY NAMES libovr libovr64 libovrkernel ovr HINTS ${OCULUS_SDK_ROOT_DIR} 
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVR/Lib/Windows/${_OCULUS_SDK_LIB_ARCH}/Release/${_OCULUS_MSVC_DIR}
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVR/Lib/Mac/Release
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVR/Lib/Linux/Release/${_OCULUS_SDK_LIB_ARCH}
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Lib/Windows/${_OCULUS_SDK_LIB_ARCH}/Release/${_OCULUS_MSVC_DIR}
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Lib/Mac/Release
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Lib/Linux/Release/${_OCULUS_SDK_LIB_ARCH}
                                                    )

#set(OCULUS_SDK_LIBRARY C:/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2013/LibOVR.lib)

# This will find release lib on Linux if no debug is available - on Linux this is no problem and avoids 
# having to compile in debug when not needed
FIND_LIBRARY(OCULUS_SDK_LIBRARY_DEBUG NAMES libovr${CMAKE_DEBUG_POSTFIX} libovr64${CMAKE_DEBUG_POSTFIX} ovr${CMAKE_DEBUG_POSTFIX} ovr libovr libovrkernel HINTS 
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVR/Lib/Windows/${_OCULUS_SDK_LIB_ARCH}/Debug/${_OCULUS_MSVC_DIR}
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVR/Lib/Mac/Debug
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVR/Lib/Mac/Release
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVR/Lib/Linux/Debug/${_OCULUS_SDK_LIB_ARCH}
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVR/Lib/Linux/Release/${_OCULUS_SDK_LIB_ARCH}
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Lib/Windows/${_OCULUS_SDK_LIB_ARCH}/Debug/${_OCULUS_MSVC_DIR}
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Lib/Mac/Debug
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Lib/Mac/Release
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Lib/Linux/Debug/${_OCULUS_SDK_LIB_ARCH}
                                                      ${OCULUS_SDK_ROOT_DIR}/LibOVRKernel/Lib/Linux/Release/${_OCULUS_SDK_LIB_ARCH}
                                                    )

#set(OCULUS_SDK_LIBRARY_DEBUG C:/OculusSDK/LibOVR/Lib/Windows/x64/Debug/VS2013/LibOVR.lib)
    
MARK_AS_ADVANCED(OCULUS_SDK_LIBRARY)
MARK_AS_ADVANCED(OCULUS_SDK_LIBRARY_DEBUG)

SET(OCULUS_SDK_LIBRARIES optimized ${OCULUS_SDK_LIBRARY} debug ${OCULUS_SDK_LIBRARY_DEBUG})

# handle the QUIETLY and REQUIRED arguments and set OCULUS_SDK_FOUND to TRUE if
# all listed variables are TRUE

#message("LIBRERIE: ${OCULUS_SDK_LIBRARIES}")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OculusSDK DEFAULT_MSG OCULUS_SDK_LIBRARIES OCULUS_SDK_INCLUDE_DIRS)

MARK_AS_ADVANCED(OCULUS_SDK_LIBRARIES OCULUS_SDK_INCLUDE_DIRS)
