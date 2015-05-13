DISTRIBUTION FOLDER
-------------------
All-comprehensive and ready-for-distribution application.
Executables and .dll, media and configuration files needed to run them will be COPIED HERE through install procedure.

DIRECTORY STRUCTURE
--------------
*root	-> copy of application + .dll
\media	-> copy of media folder
\cfg	-> contains needed .cfg files (only release versions!)

HOW TO
------
1. Create project/solution for the desired platform with cmake
2. Compile the project/solution in "Release" configuration
3. Launch the install procedure
	- es. on VS, compile the INSTALL project
	- es. with makefile, run "make install"
4. Cut/copy/paste/rename "dist" folder wherever/how you like - application will always run if:
	a. .cfg files are properly configured for the system the app runs
	b. environment variable "OGRE_HOME" is properly set if app accesses data directly from OgreSDK

TIPS
----
* Don't change default .cfg file names (cmake relies on that!)
* Only plugins.cfg will be copied (not plugins_d.cfg) and scanned for needed .dlls to be copied
* Copy all needed material to local /media folder before installing