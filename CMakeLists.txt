cmake_minimum_required(VERSION 2.6)

project(OculusOgreSample)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if (NOT DEFINED ENV{OGRE_HOME} AND NOT DEFINED OGRE_HOME)
	message(SEND_ERROR "Must define OGRE_HOME.")
else()
	if (NOT DEFINED OGRE_HOME)
		set(OGRE_HOME ENV{OGRE_HOME})
	endif()
	if (NOT DEFINED OGRE_BUILD OR "${OGRE_BUILD}" STREQUAL "")
		set(OGRE_BUILD ${OGRE_HOME})
	endif()
	if( NOT DEFINED OGRE_SOURCE_DIR OR "${OGRE_SOURCE_DIR}" STREQUAL "")
		set(OGRE_SOURCE_DIR ${OGRE_HOME})
	endif()
endif()

if (NOT DEFINED ENV{OCULUS_HOME} AND NOT DEFINED OCULUS_HOME)
	if( NOT EXISTS "${CMAKE_SOURCE_DIR}/OculusSDK")
		message(SEND_ERROR "Must define OCULUS_HOME, or put the OculusSDK folder into the Project's root directory. Make sure the name of the folder is 'OculusSDK'")
	endif()
else()
	set(OCULUS_HOME ENV{OCULUS_HOME})
endif()

set(OIS_AND_OGRE_LIB_DIRS
	${OGRE_HOME}/lib/debug
	${OGRE_HOME}/lib/Release
	${OGRE_HOME}/bin/debug
	${OGRE_HOME}/bin/Release
)

# Get the Packages (finds the library and its include files):
find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)

if( DEFINED OCULUS_HOME )
	set( OCULUS_SDK_ROOT_DIR ${OCULUS_HOME} )
else()
	set(OCULUS_SDK_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/OculusSDK )
endif()
find_package(OculusSDK REQUIRED)


message(STATUS OIS_LIBRARIES)
message(STATUS ${OIS_LIBRARIES})
message(STATUS ${OIS_LIBRARYDIR})
message(STATUS ${OIS_INCLUDEDIR})

include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OIS_INCLUDE_DIRS}
	${OGRE_Overlay_INCLUDE_DIRS}
	${OGRE_SAMPLES_INCLUDEPATH}
	${OCULUS_SDK_INCLUDE_DIRS}
	include
	src
)

link_directories(
	${OIS_AND_OGRE_LIB_DIRS}
	${Boost_LIBRARY_DIRS}
)

set(HDRS
	include/App.h
	include/Scene.h
	include/Rift.h
)
 
set(SRCS
	main.cpp
	src/App.cpp
	src/Scene.cpp
	src/Rift.cpp
)

add_executable(OculusOgreSample ${HDRS} ${SRCS})
target_link_libraries(OculusOgreSample
	${OGRE_LIBRARIES}
	${OGRE_Overlay_LIBRARIES}
	${OIS_LIBRARIES}
	${OCULUS_SDK_LIBRARIES}
)
if( WIN32 )
	target_link_libraries(OculusOgreSample
		winmm.lib
		ws2_32.lib
	)
endif()

# Copy files needed by Ogre3D (plugins.cfg and resources.cfg)
message(STATUS "Ogre plugin dir found: ${OGRE_PLUGIN_DIR_REL}")
file(COPY plugins.cfg DESTINATION . )
if (OGRE_RenderSystem_GL_FOUND)
	# Change path to Ogre in plugins.cfg:
	file(READ ${PROJECT_BINARY_DIR}/plugins.cfg PLUGINS_FILE)

	string(REGEX REPLACE "PluginFolder=[^\n]*"
		"PluginFolder=./Plugins" PLUGINS_FILE_MODIFIED
		${PLUGINS_FILE})
	file(WRITE ${PROJECT_BINARY_DIR}/plugins.cfg ${PLUGINS_FILE_MODIFIED})
endif()

file(COPY resources.cfg DESTINATION . )
file(COPY media DESTINATION . )

if( WIN32 )
	# /MP for multiple thread compiling
	# WIN32_LEAN_AND_MEAN to avoid conflicts between boost and Leap motion, which both seem to include winsock.h
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /DWIN32_LEAN_AND_MEAN")
	
	# Copy the Ogre DLLs:
	file(COPY ${OGRE_HOME}/bin/debug/ DESTINATION ${PROJECT_BINARY_DIR}/Debug FILES_MATCHING PATTERN "*.dll")
	file(COPY ${OGRE_HOME}/bin/Release/ DESTINATION ${PROJECT_BINARY_DIR}/Debug FILES_MATCHING PATTERN "*.dll")
	file(COPY ${OGRE_HOME}/bin/Release/ DESTINATION ${PROJECT_BINARY_DIR}/Release FILES_MATCHING PATTERN "*.dll")

endif()