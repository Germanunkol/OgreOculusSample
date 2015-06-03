# -------------------------------
# Global variables used here are:
#	- ${CMAKE_SOURCE_DIR}				(globally configured by cmake)
# 	- ${CMAKE_INSTALL_PREFIX} 			(globally configured by cmake)
set (OGRE_PLUGIN_DIR_REL "@OGRE_PLUGIN_DIR_REL@")		# values mirrored by configure_file()
set (OGRE_MEDIA_FOLDER_NAME "@OGRE_MEDIA_FOLDER_NAME@")	# values mirrored by configure_file()
set (config_dir_name "@config_dir_name@")				# values mirrored by configure_file()
set (plugins_dir_name "@plugins_dir_name@")				# values mirrored by configure_file()

#
# -------------------------------


# Set the variable "Esc" to some ASCII value (basically something
# which is unlikely to conflict with anything in the file contents)
# to replace \n on line extraction from text files
string(ASCII 27 Esc)


# 1 - copy only needed plugin DLLs (to a path relative to the executable) parsing plugins.cfg
# ----------------------------
# http://stackoverflow.com/questions/30226775/excluding-lines-containing-a-specific-string-from-regex-replace-in-cmake
file(READ ${CMAKE_INSTALL_PREFIX}/${config_dir_name}/plugins.cfg PLUGINS_FILE)
# Turn the contents into a list of strings, each ending with an Esc.
# This allows us to preserve blank lines in the file since CMake
# automatically prunes empty list items during a foreach loop.
string(REGEX REPLACE "\n" "${Esc};" ContentsAsList "${PLUGINS_FILE}")
foreach(Line ${ContentsAsList})
  # Don't modify the line if it contains # at the beginning
  if(NOT "${Line}" MATCHES "^#")	
    string(REGEX MATCH "Plugin=(.+)${Esc}$" result ${Line})	# now CMAKE_MATCH_0 contains the whole match, as in "result"
    if(NOT ${CMAKE_MATCH_1} STREQUAL "")					# and CMAKE_MATCH_1 contains the submatch between ()
    	set (plugin_name ${CMAKE_MATCH_1})					
    	message(STATUS "Installing plugin '${plugin_name}'...")
    	file(COPY "${OGRE_PLUGIN_DIR_REL}/${plugin_name}.dll"
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${plugins_dir_name}
		)
		message(STATUS "Plugin '${plugin_name}' installed.")
    endif()
  endif()
endforeach()
message(STATUS "All active plugins from plugins.cfg installed.")




# 2 - make path to Ogre plugins in plugins.cfg relative
# ------------------------------
string(REGEX REPLACE "PluginFolder=[^\n]*"
	"PluginFolder=${plugins_dir_name}" PLUGINS_FILE_MODIFIED ${PLUGINS_FILE})
file(WRITE ${CMAKE_INSTALL_PREFIX}/${config_dir_name}/plugins.cfg ${PLUGINS_FILE_MODIFIED})
message(STATUS "Plugin folder reference in plugins.cfg updated.")



# 3 - 	copy only needed OGRE media parsing resources.cfg
# 		(local media has already been installed by main CMakeLists script!)
# 4 - 	update all paths to Ogre media in resources.cfg and make them relative
# ------------------------------
file(READ ${CMAKE_INSTALL_PREFIX}/${config_dir_name}/resources.cfg RESOURCES_FILE)
# http://stackoverflow.com/questions/30226775/excluding-lines-containing-a-specific-string-from-regex-replace-in-cmake
# Turn the contents into a list of strings, each ending with an Esc.
# This allows us to preserve blank lines in the file since CMake
# automatically prunes empty list items during a foreach loop.
string(REGEX REPLACE "\n" "${Esc};" ContentsAsList "${RESOURCES_FILE}")
unset(RESOURCES_FILE_MODIFIED)	# empty the variable if previously used..
foreach(Line ${ContentsAsList})
	# extracting path information (SKIP LOCAL PATHS -- [^.]+ means any string not containing "." which are locals)
	string(REGEX MATCH "=([^.]+)/(${OGRE_MEDIA_FOLDER_NAME}.*)${Esc}$" result ${Line})
	# now CMAKE_MATCH_0 contains the whole match, as in "result"
	if(NOT ${CMAKE_MATCH_0} STREQUAL "")	# --> result is not empty = a global path is found
		# parsing source path info
		set (resource_path_prefix ${CMAKE_MATCH_1})					# and CMAKE_MATCH_1 contains the submatch between first ()
		set (resource_path_suffix ${CMAKE_MATCH_2})					# and CMAKE_MATCH_2 contains the submatch between second ()
		set (resource_full_path "${CMAKE_MATCH_1}/${CMAKE_MATCH_2}")
		# parsing destination path info
		string(REGEX REPLACE "/[^/]+$" "" resource_destination_folder ${resource_path_suffix})	# removes last "/suffix" from paths, if any
		# copying the whole tree to destination media folder
		message(STATUS "Installing resources from '${resource_full_path}' ...")
		file(COPY "${resource_full_path}"
			DESTINATION "${CMAKE_INSTALL_PREFIX}/${resource_destination_folder}"
		)
		message(STATUS "Resources installed in subfolder '/${resource_path_suffix}'.")
	endif()
	# update line so that they start with "media" (relative path)
	#message(STATUS "Updating reference in resources.cfg ...")
	string(REGEX REPLACE "=.*/media" "=media" Line ${Line})
	# Swap the appended Esc character back out in favour of a line feed
	string(REGEX REPLACE "${Esc}" "\n" Line ${Line})
	# Append modified line to output file
	set(RESOURCES_FILE_MODIFIED "${RESOURCES_FILE_MODIFIED}${Line}")
endforeach()
file(WRITE ${CMAKE_INSTALL_PREFIX}/${config_dir_name}/resources.cfg ${RESOURCES_FILE_MODIFIED})
message(STATUS "All resources copied and references in resources.cfg updated.")
message("NOTICE: it may be possible that some files/folders won't copy due to permissions/restrictions. Please check.")

