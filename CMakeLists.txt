cmake_minimum_required(VERSION 2.6)
set(cmake_test_VERSION_MAJOR 0)
set(cmake_test_VERSION_MINOR 1)
set(cmake_test_VERSION_PATCH 0)


# Cmake template for generic projects by Dario Facchini
# Current implementation: OculusSDK 0.5 (static linking), OgreSDK 1.9 (dynamic linking)
# - - - - - - - - - - - - - - - - - - - - - - - - - - -
# WINDOWS: to make application find mandatory .dll (not the plugins), just add Ogre bin directories to PATH
# WINDOWS: to run this cmake, please set OGRE_HOME as environment variable to your OgreSDK main folder
# WINDOWS: to run this cmake, please set OCULUS_HOME as environment variable to your OculusSDK main folder
# - - - - - - - - - - - - - - - - - - - - - - - - - - -
# FULLY AND EASILY CUSTOMIZABLE directory names
# "CONFIGURE" and "GENERATE" a custom project in a subdirectory (the "working directory") of cmake current folder
# AUTOFINDS include (.h and .hpp) and source files (.cpp) recursively in the dedicated directories
# BUILD FILES are saved in a dedicated directory (different from project subfolder)
# DEDICATED distribution FOLDER (build, configuration and media files are automatically copied on INSTALL)
# ON "INSTALL" - for Ogre:
#	- release executable is copied (if existing)
#	- all needed release .dll are copied to distribution dir
# 	- all needed release plugins .dll (defined in plugins.cfg) are copied to plugins distribution subdir
#	- all needed resources (defined in resources.cfg, both from Ogre folder and from media folder) are copied to media distribution subdir
#	- all release configuration files are copied in config distribution subdir and updated to reference the copied files
# A PostInstall.cmake script template is used to parse configuration files and copy all contents.



################################################################
#                   PROJECT SETTINGS (CUSTOMIZE THIS)          #
################################################################
# Please refer to the README before modifying!!

# project name
project (OgreOculusSample)

# all directories names relative to CMAKE_SOURCE_DIR
set (libs_dir_name	 "libs")		# name of the dir containing ALL subprojects/libs to be compiled toghether with this project
set (source_dir_name "src")			# name of the dir containing ALL implementations
set (header_dir_name "include")		# name of the dir containing ALL declarations
set (binary_dir_name "build")		# name of the dir that will contain build results (Release, Debug, ..)
set (media_dir_name  "media")		# name of the dir containing LOCAL media (not Ogre's or any other 3rd party app)
									# ALSO the name of the dir where both LOCAL and GLOBAL media will be INSTALLED in /dist
									# WARNING: SET REFERENCES IN PROGRAM OR ANY CONFIGURATION FILES ACCORDINGLY!!
set (config_dir_name "cfg")			# name of the dir containing CONFIGURATION FILES (Ogre's or any other)
									# ALSO the name of the dir where .cfg files will be INSTALLED in /dist
									# WARNING: SET REFERENCES IN PROGRAM ACCORDINGLY!!
set (plugins_dir_name "plugins")	# name of the subdir that will contain plugins on INSTALL
set (distribution_dir_name "dist")	# name of the dir that contains final distribution files, such as:
									#	- final release executable files and .dlls (in root)
									#	- data/media files (in /media_dir_name)
									#	- runtime plugins (in /plugin_dir_name)
									#	- configuration files (in /config_dir_name)
set (cmake_dir_name "cmake")		# name of the dir containing cmake files for this project
									# N.B.	cmake will search in the standard cmake directory,
									#		then in this directory,
									#		then will search into sdk own cmake directories (if they have any)
set (exe_name "OgreOculusSample")		# mame of executable
set (install_script_name "PostInstall")	# name of the TEMPLATE cmake file for generating install script
										# all @VARS@ in this file will be replaced with current specific values
										# at the end of this script with configure_file() and then used at install time.
										# This is done using local variable values in an install script!


################################################################
#                   PROJECT/BUILD SETUP     		           #
################################################################

# cmake project variables
set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}						# (this is the standard cmake directory)
	"${CMAKE_SOURCE_DIR}/${cmake_dir_name}" 	# append: where personal project cmake files are stored
	)
include_directories("${PROJECT_BINARY_DIR}")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/${binary_dir_name})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
unset(LIBS_TO_LINK)

# cmake build variables
if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# cmake install variables
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/${distribution_dir_name}" CACHE PATH "${exe_name} default install path" FORCE )
endif()
set(CMAKE_INSTALL_SCRIPT "${CMAKE_SOURCE_DIR}/${cmake_dir_name}/${install_script_name}.cmake")	# this is cmake template for generating CMAKE_INSTALL_SCRIPT_SPECIFIC
set(CMAKE_INSTALL_SCRIPT_SPECIFIC "${CMAKE_SOURCE_DIR}/${cmake_dir_name}/${install_script_name}.specific.cmake") # result script with hardcoded variable values


################################################################
#                   OGRE SETUP	                               #
################################################################

if(WIN32)

	# check if OGRE_HOME is defined
	if (NOT DEFINED ENV{OGRE_HOME} AND NOT DEFINED OGRE_HOME)
		message(SEND_ERROR "Must define OGRE_HOME (as Environment variable or cmake variable). You can also set it manually now and run Configure again.")
		# give the option to setup OCULUS_HOME manually
		set(OGRE_HOME "${OGRE_HOME}" CACHE PATH "Location of your Ogre libraries")
	# if at least one of them is defined, use it
	else()
		if (NOT DEFINED OGRE_HOME)
			message("Environment variable OGRE_HOME found. This will be used as OgreSDK folder.")
			set(OGRE_HOME $ENV{OGRE_HOME})
		endif()
		if (NOT DEFINED OGRE_BUILD OR "${OGRE_BUILD}" STREQUAL "")
			set(OGRE_BUILD ${OGRE_HOME})
		endif()
		if( NOT DEFINED OGRE_SOURCE_DIR OR "${OGRE_SOURCE_DIR}" STREQUAL "")
			set(OGRE_SOURCE_DIR ${OGRE_HOME})
		endif()

	endif()

	# set cmake directory for OGRE (when a findOgre.cmake is not found in local "cmake" dir)
	set(CMAKE_MODULE_PATH "${OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")	# append: OGRE personal cmake directory
	set(OGRE_SAMPLES_INCLUDEPATH "${OGRE_HOME}/Samples/Common/include")	

endif(WIN32)

if(UNIX)

	# look for cmake directory for OGRE (different options assumed here)
	if(EXISTS "/usr/local/lib/OGRE/cmake")
 
	  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/") # We could just *assume* that developers uses this basepath : /usr/local
 
	elseif(EXISTS "/usr/lib/OGRE/cmake")
 
	  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one
 
	else ()
	  message(SEND_ERROR "Failed to find cmake module path.")
	endif(EXISTS "/usr/local/lib/OGRE")

endif(UNIX)

# force OGRE_SDK_ROOT_DIR to show OGRE_HOME value
#set(OGRE_SDK_ROOT_DIR "${OGRE_HOME}" CACHE PATH "Location of your Ogre3D libraries" FORCE)
	

################################################################
#                   OCULUSSDK SETUP	                           #
################################################################

# check if there is a bundled version of OculusSDK in this project lib folder
# NOTE:	default is giving priority to the bundled version, this is because it can be specifically tweaked for this project.
#		If you don't like it, just : project will work, but with less functionality
if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${libs_dir_name}/OculusSDK")
	message("Bundled OculusSDK found in '/${libs_dir_name}'.")
	set (BUNDLED_OculusSDK_FOUND true)
	# give the option to use it (true by default)
	set (USE_BUNDLED_OculusSDK true CACHE BOOL "Use the version of OculusSDK bundled in this project (within '/${libs_dir_name}' folder)" )
else()
	message(STATUS "No bundled OculusSDK found in '/${libs_dir_name}'.")
	set (BUNDLED_OculusSDK_FOUND false)
	# don't show any option
	set (USE_BUNDLED_OculusSDK false)
endif()


if(WIN32)
	# if using bundled OculusSDK
	if(BUNDLED_OculusSDK_FOUND AND USE_BUNDLED_OculusSDK)
		# set OCULUS_HOME cmake variable as OculusSDK local path
		set(OCULUS_HOME "${CMAKE_CURRENT_SOURCE_DIR}/${libs_dir_name}/OculusSDK")
		message("Found a bundled OculusSDK in '${OCULUS_HOME}'.")
		if(DEFINED ENV{OCULUS_HOME})
			message("Overriding OculusSDK from 'ENV{OCULUS_HOME}'. Uncheck 'USE_BUNDLED_OculusSDK' flag if you wish to use the version in your system.")
		endif()
		# find out if source code of bundled OculusSDK is also available
		if( EXISTS "${OCULUS_HOME}/LibOVR/Src/OVR_CAPI.cpp")
			set (OculusSDK_SOURCE_FOUND true)
			# give the option to compile it toghether with the project
			set (COMPILE_OculusSDK false CACHE BOOL "Compile the source of OculusSDK toghether with the project" )
		else()
			set (OculusSDK_SOURCE_FOUND false)
		endif()
	# else, USE STANDARD INSTALLATION OculusSDK
	else()
		# hide any option for compiling
		unset(COMPILE_OculusSDK CACHE)
		set (COMPILE_OculusSDK false)
		# if OCULUS_HOME is NOT defined (environment or cmake variable)
		if (NOT DEFINED ENV{OCULUS_HOME} AND NOT DEFINED OCULUS_HOME)
			# throw ERROR
			message(SEND_ERROR "Can't find OculusSDK. Must define OCULUS_HOME (as Environment variable or cmake variable), or put 'OculusSDK' folder into '${libs_dir_name}' directory. You can also set it manually now and run Configure again.")
			# give the option to setup OCULUS_HOME manually
			set(OCULUS_HOME "${OCULUS_HOME}" CACHE PATH "Location of your OculusSDK libraries")
		# if one of them is defined, use it	
		else()
			if (NOT DEFINED OCULUS_HOME)
				message(STATUS "Environment variable OCULUS_HOME found. This will be used as OculusSDK folder.")
				# assign OCULUS_HOME the value of environment variable
				set(OCULUS_HOME $ENV{OCULUS_HOME})
			endif()
			message("Using OculusSDK from '${OCULUS_HOME}'. Check 'USE_BUNDLED_OculusSDK' flag if you wish to use the version bundled in the project.")
		endif()
	endif()

	# add OculusSDK cmake directory to .cmake search path
	set(CMAKE_MODULE_PATH "${OCULUS_HOME}/CMake/;${CMAKE_MODULE_PATH}")				# append: OCULUSSDK personal cmake directory

	# set always OCULUS_SDK_ROOT_DIR as OCULUS_HOME
	set(OCULUS_SDK_ROOT_DIR ${OCULUS_HOME})

endif()

if(UNIX)	
	# still experimenting on unix..
endif(UNIX)


# force OCULUS_SDK_ROOT_DIR to show OCULUS_HOME value
#set(OCULUS_SDK_ROOT_DIR "${OCULUS_HOME}" CACHE PATH "Location of your OculusSDK libraries" FORCE)
	


################################################################
#                   EXTERNAL DEPENDENCIES                      #
################################################################

# OIS + OGRE lib
find_package(OIS REQUIRED)							# OIS lib config
find_package(OGRE REQUIRED)							# OGRE lib config
list(APPEND LIBS_TO_LINK							# OGRE + OIS lib link
	${OGRE_LIBRARIES}								
	${OGRE_Overlay_LIBRARIES}						
	${OIS_LIBRARIES}								
	)
include_directories(								# OGRE + OIS lib include
	"${OIS_INCLUDE_DIRS}"
	"${OGRE_INCLUDE_DIRS}"
	"${OGRE_Overlay_INCLUDE_DIRS}"
	"${OGRE_SAMPLES_INCLUDEPATH}"
	)

# OculusSDK lib
unset(OCULUSSDK_FOUND CACHE)
set(OCULUSSDK_FOUND false)
if(USE_BUNDLED_OculusSDK)
	find_package(OculusSDK QUIET)					# OculusSDK lib config
	#message(${OCULUSSDK_FOUND})
	if(NOT OCULUSSDK_FOUND)
		if(OculusSDK_SOURCE_FOUND)
			message("Failed to find bundled OculusSDK precompiled libraries. Source code available. Will force to compile it.")
			set (COMPILE_OculusSDK true CACHE BOOL "[FORCED] Compile the source of OculusSDK toghether with the project" FORCE)
		else()
			message(SEND_ERROR "Failed to find bundled OculusSDK libraries/source.")
		endif()
	endif()	
else()
	find_package(OculusSDK REQUIRED)
endif()

if(NOT COMPILE_OculusSDK)					
	list(APPEND LIBS_TO_LINK ${OCULUS_SDK_LIBRARIES})# OculusSDK lib link
	include_directories(							# OculusSDK lib include
	"${OCULUS_SDK_INCLUDE_DIRS}"
	)
endif()

							
# OPENCV lib
find_package(OpenCV REQUIRED)						# OpenCV lib config
list(APPEND LIBS_TO_LINK ${OpenCV_LIBS})			# OpenCV lib link
include_directories(								# OpenCV lib include
	"${OpenCV_INCLUDE_DIRS}"
	)



################################################################
#                   MAIN PROJECT SOURCE DIR                    #
################################################################
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${source_dir_name})
include_directories("${PROJECT_SOURCE_DIR}")

################################################################
#                   MAIN PROJECT INCLUDE DIR 				   #
################################################################
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${header_dir_name})
include_directories("${PROJECT_INCLUDE_DIR}")

################################################################
#                   ADD MAIN PROJECT FILES 					   #
################################################################
file(GLOB_RECURSE source_files "${source_dir_name}/*.cpp")
source_group("${source_dir_name}" FILES ${source_files})
file(GLOB_RECURSE header_files "${header_dir_name}/*.hpp" "${header_dir_name}/*.h")
source_group("${header_dir_name}" FILES ${header_files})

################################################################
#                   ADD SUBPROJECTS (RECURSIVE)				   #
################################################################

# OculusSDK lib - add as SUBPROJECT if user wants to compile it from bundled version
if(USE_BUNDLED_OculusSDK AND COMPILE_OculusSDK)
	add_subdirectory(${OCULUS_HOME}/LibOVR)				# OculusSDK subproject source
	set_target_properties(ovr PROPERTIES FOLDER "3rdParty")
	list(APPEND LIBS_TO_LINK ovr)						# OculusSDK subproject lib link4
	include_directories(								# OculusSDK subproject include
		"${OCULUS_HOME}/LibOVR/Include"
		"${OCULUS_HOME}/LibOVRKernel/Src"
	)
endif()



################################################################
#                   EXE OUTPUT                                 #
################################################################
ADD_EXECUTABLE("${exe_name}" ${source_files} ${header_files})

################################################################
#                   LINKING LIBRARIES                          #
################################################################

# All platforms
target_link_libraries("${exe_name}"
	${LIBS_TO_LINK}
	#${Boost_LIBRARY_DIRS}							# Boost libraries (if your Ogre needs it)
)					

# Win32 specific
if( WIN32 )
	target_link_libraries("${exe_name}"
		winmm.lib
		ws2_32.lib
	)
# Unix specific
elseif( UNIX )

		# Where to find UDEV:
		find_package(UDEV REQUIRED)
		# Where to find XINERAMA:
		find_package(Xinerama REQUIRED)
		find_package(X11 REQUIRED)

	target_link_libraries("${exe_name}"
		librt.so
		libXrandr.so
		libGL.so
		libXxf86vm.so
		libpthread.so
		${XINERAMA_LIBRARY}
		${X11_LIBRARIES}
	)
endif()

# set "_d" postfix for debug configurations
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX _d)

# /MP for multiple thread compiling
# WIN32_LEAN_AND_MEAN to avoid conflicts between boost and Leap motion, which both seem to include winsock.h
if( WIN32 )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /DWIN32_LEAN_AND_MEAN")
endif()


################################################################
#                   OGRE RUNTIME FILES CONFIG                  #
################################################################
message(STATUS "Ogre Debug plugin dir found: ${OGRE_PLUGIN_DIR_DBG}")
message(STATUS "Ogre Release plugin dir found: ${OGRE_PLUGIN_DIR_REL}")

# Set path to Ogre plugins in plugins.cfg/plugins_d.cfg according to your system:
# -------------------------
file(READ ${CMAKE_SOURCE_DIR}/${config_dir_name}/plugins.cfg PLUGINS_FILE)
file(READ ${CMAKE_SOURCE_DIR}/${config_dir_name}/plugins_d.cfg PLUGINS_D_FILE)
string(REGEX REPLACE "PluginFolder=[^\n]*"
	"PluginFolder=${OGRE_PLUGIN_DIR_REL}" PLUGINS_FILE_MODIFIED
	${PLUGINS_FILE})
string(REGEX REPLACE "PluginFolder=[^\n]*"
	"PluginFolder=${OGRE_PLUGIN_DIR_DBG}" PLUGINS_D_FILE_MODIFIED
	${PLUGINS_D_FILE})
file(WRITE ${CMAKE_SOURCE_DIR}/${config_dir_name}/plugins.cfg ${PLUGINS_FILE_MODIFIED})
file(WRITE ${CMAKE_SOURCE_DIR}/${config_dir_name}/plugins_d.cfg ${PLUGINS_D_FILE_MODIFIED})
message(STATUS "plugins.cfg and plugins.cfg updated according to your setup.")

# Set absolute paths to Ogre media in resources.cfg according to your system:
# ---------------------------
# http://stackoverflow.com/questions/30226775/excluding-lines-containing-a-specific-string-from-regex-replace-in-cmake
string(REGEX REPLACE	# regex does not like escapes :)
	"\\\\"				# for regex this means "\"
	"/"
	OGRE_HOME_BACKSLASHES ${OGRE_HOME})
file(READ ${CMAKE_SOURCE_DIR}/${config_dir_name}/resources.cfg RESOURCES_FILE)
# Set the variable "Esc" to some ASCII value - basically something
# which is unlikely to conflict with anything in the file contents.
string(ASCII 27 Esc)
# Turn the contents into a list of strings, each ending with an Esc.
# This allows us to preserve blank lines in the file since CMake
# automatically prunes empty list items during a foreach loop.
string(REGEX REPLACE "\n" "${Esc};" ContentsAsList "${RESOURCES_FILE}")
unset(RESOURCES_FILE_MODIFIED)	# empty the variable if previously used..
foreach(Line ${ContentsAsList})
  # Don't modify the line if resource path starts with one ore more "."
  if(NOT "${Line}" MATCHES "^FileSystem=[.]+")
  	# WE MAKE AN ASSUMPTION: Ogre global media files were in a path that contained the word "Media"
  	# so we assume that any modifications made to this file by you contain this word.
  	# But.. this MAY CHANGE in the future! So, to make it stronger, we try with "media", "Media" and "MEDIA".
  	# Paths not containing these words remain UNTOUCHED
    string(REGEX REPLACE "=.*/media" "=${OGRE_MEDIA_DIR}" Line ${Line})	# media
    string(REGEX REPLACE "=.*/Media" "=${OGRE_MEDIA_DIR}" Line ${Line})	# Media (most likely)
    string(REGEX REPLACE "=.*/MEDIA" "=${OGRE_MEDIA_DIR}" Line ${Line})	# MEDIA
    # now all those paths REFERENCE CORRECTLY the Ogre media folder installed in your system!																			
  endif()
  # Swap the appended Esc character back out in favour of a line feed
  string(REGEX REPLACE "${Esc}" "\n" Line ${Line})
  # Append modified line to output file
  set(RESOURCES_FILE_MODIFIED "${RESOURCES_FILE_MODIFIED}${Line}")
endforeach()
file(WRITE ${CMAKE_SOURCE_DIR}/${config_dir_name}/resources.cfg ${RESOURCES_FILE_MODIFIED})
message(STATUS "resources.cfg updated according to your setup.")

# ok.. but now what is the name of YOUR Ogre media folder?
# better if we save it: we must use it to identify the absolute part of the paths
# and strip it when INSTALL is performed (and convert the path from absolute to relative)
# see PostInstall.cmake file for details on how I do it :)
string(REGEX MATCH "([^.]+)/([^/]+)$" OGRE_MEDIA_FOLDER_NAME ${OGRE_MEDIA_DIR})
set(OGRE_MEDIA_FOLDER_NAME ${CMAKE_MATCH_2})
# now OGRE_MEDIA_FOLDER_NAME contains the word extracted after the last "/" found



################################################################
#                   POST-BUILD AND INSTALL 					   #
################################################################
# Run "INSTALL" to copy exe, dll, media and cfg files into /dist folder

# define "dist" directory where finished project should be installed for final distribution
#if(WIN32 AND NOT MINGW)
#	add_custom_command( TARGET ${PROJECT_NAME} PRE_BUILD
#		COMMAND if not exist ..\\${distribution_dir_name} mkdir ..\\${distribution_dir_name} )
	#add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
	#	COMMAND copy \"$(TargetPath)\" ..\\${distribution_dir_name} )
#endif(WIN32 AND NOT MINGW)
 
#if(MINGW OR UNIX)
	#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${distribution_dir_name})
#endif(MINGW OR UNIX)

# 
#install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/PreInstall.cmake")

# copy release executable
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
	CONFIGURATIONS Release RelWithDebInfo
)

# copy LOCAL /media folder AS IS into /dist (IF ANY!)
if(EXISTS "${CMAKE_SOURCE_DIR}/${media_dir_name}" AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${media_dir_name}")
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/${media_dir_name}
		DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
endif()

# [OGRE] copy needed cfg files from /cfg(MUST BE DONE BEFORE RUNNING PostInstall SCRIPT)
if(EXISTS "${CMAKE_SOURCE_DIR}/${config_dir_name}/plugins.cfg" AND EXISTS "${CMAKE_SOURCE_DIR}/${config_dir_name}/resources.cfg")
	install(FILES
		${CMAKE_SOURCE_DIR}/${config_dir_name}/plugins.cfg
		${CMAKE_SOURCE_DIR}/${config_dir_name}/resources.cfg
		DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}/${config_dir_name}
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
endif()

# [OGRE] copy all MANDATORY DLLs for OGRE (to the same path of the executable)
# NOTE: from the 1.7.1 sdk the OIS dll is called OIS.dll instead of libOIS.dll
# so you'll have to change that to make it work with 1.7.1
# DON'T INCLUDE HERE ANY PLUGIN DEFINED IN PLUGINS.CFG !!
install(FILES
	${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
	${OGRE_PLUGIN_DIR_REL}/OIS.dll
	${OGRE_PLUGIN_DIR_REL}/cg.dll
	${OGRE_PLUGIN_DIR_REL}/OgreOverlay.dll
	DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
	CONFIGURATIONS Release RelWithDebInfo Debug
)

# Boost management: NOT TESTED - feel free to improve it!
if(Boost_FOUND)

	if(WIN32)
	 
		# NOTE: as of sdk 1.7.2 we need to copy the boost dll's as well
		# because they're not linked statically (it worked with 1.7.1 though)
		install(FILES ${Boost_DATE_TIME_LIBRARY_RELEASE}
			${Boost_THREAD_LIBRARY_RELEASE}
			DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
			CONFIGURATIONS Release RelWithDebInfo
		)
		install(FILES ${Boost_DATE_TIME_LIBRARY_DEBUG}
			${Boost_THREAD_LIBRARY_DEBUG}
			DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
			CONFIGURATIONS Debug
		)	
	 
	endif(WIN32)

	# Copy the Boost libraries:
	get_filename_component( DIR ${Boost_SYSTEM_LIBRARY_DEBUG} DIRECTORY )
	get_filename_component( S ${Boost_SYSTEM_LIBRARY_DEBUG} NAME_WE )

	add_custom_command(TARGET OculusOgreSample POST_BUILD        # Adds a post-build event to MyTest
		COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
			"${Boost_SYSTEM_LIBRARY_DEBUG}"      # <--this is in-file
			$<TARGET_FILE_DIR:OculusOgreSample>)                 # <--this is out-file path               # <--this is out-file path
			
	add_custom_command(TARGET OculusOgreSample POST_BUILD        # Adds a post-build event to MyTest
		COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
			"${DIR}/${S}.dll"      # <--this is in-file
			$<TARGET_FILE_DIR:OculusOgreSample>)                 # <--this is out-file path

	get_filename_component( DIR ${Boost_SYSTEM_LIBRARY_RELEASE} DIRECTORY )
	get_filename_component( S ${Boost_SYSTEM_LIBRARY_RELEASE} NAME_WE )

	add_custom_command(TARGET OculusOgreSample POST_BUILD        # Adds a post-build event to MyTest
		COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
			"${Boost_SYSTEM_LIBRARY_RELEASE}"      # <--this is in-file
			$<TARGET_FILE_DIR:OculusOgreSample>)                 # <--this is out-file path
			
	add_custom_command(TARGET OculusOgreSample POST_BUILD        # Adds a post-build event to MyTest
		COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
			"${DIR}/${S}.dll"      # <--this is in-file
			$<TARGET_FILE_DIR:OculusOgreSample>)                 # <--this is out-file path

endif()

# [OGRE] Generate an install script to be run at install time to:
#	1 - copy only NEEDED PLUGIN DLLs (parsing plugins.cfg) such as RenderSystems to /plugins
# 	2 - set path to Ogre plugins in plugins.cfg to RELATIVE
#	3 - copy only needed media (parsing resources.cfg) from Ogre media directory to /media
#	4 - set paths to Ogre media in resources.cfg to RELATIVE
configure_file(${CMAKE_INSTALL_SCRIPT} ${CMAKE_INSTALL_SCRIPT_SPECIFIC} @ONLY)	# this command allows to mirror variables from this file to another
																				# @ONLY means only @VARS@ (and not ${VARS}) will be replaced in destination file!
install (SCRIPT ${CMAKE_INSTALL_SCRIPT_SPECIFIC})

