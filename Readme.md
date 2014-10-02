Oculus Rift Rendering in Ogre
=============================

TODO:
-------------

- [Done] Add distortion
- [Done] Add command line switch to enable window rotation
- Add Command line switch to control which monitor the screen goes to
- Add time warp
- Distortion seems to be off - might be fixed with time warp...?
- Add Position tracking
- Add License

Note:
-------------

At the point of writing, the Oculus DK2 only works on Windows, so this project has not been tested
on other platforms. However, all used libraries run on Win, Linux and Mac, so it should be easy enough to
port once the Oculus SDK has been released for those platforms.

Goal:
-------------

- Rendering to the Oculus Rift using extended mode.
- Should work with DK2

Prerequisites:
-------------

- Ogre must be installed (Tested with version 1.9)
- OculusSDK (download from Oculus Rift Website - Tested with version 0.4.2)
- Install Oculus Runtime

Building:
-------------

- Use cmake to build in directory of choice.
- WIN: Create variable OGRE_HOME and set to the place where you've installed or built OGRE
- Put the OculusSDK folder into the source folder, or set OCULUS_HOME to point to the OculusSDK folder.
- If your version of OGRE was built with boost (i.e. threadding was enabled) then you'll also need boost (get binaries online). Make sure to install a version for the Compiler you're using (for example: Visual C++ 2010 Express).
- WIN: If using boost, set the BOOST_ROOT to C:\local\boost_1_56_0 (or wherever you installed boost)
- WIN: If using boost, set the BOOST_INCLUDEDIR to C:\local\boost_1_56_0 (or wherever you installed boost)
- WIN: If using boost, set the BOOST_LIBRARYDIR to C:\local\boost_1_56_0\lib32-msvc-10.0 (or wherever you installed boost)
