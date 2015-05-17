// ----------------------------------------------------------------------
// Visualization of Patient data (Dicoms and surface models) using the
// Oculus Rift.
// ---------------------------------------------------------------------
// August 2014, Micha Pfeiffer
// May 2015, Dario Facchini
// Couples Ogre3D and the Oculus Rift SDK to render Meshes in a VR setting
//

#include <iostream>
#include "Globals.h"

#include "App.h"

extern "C" {

    int main(int argc, char *argv[])
    {
		std::cout << "Hello world." << std::endl;

		for ( int i = 0; i < argc; i++ )
		{
			std::string arg( argv[i] );

			// This flag triggers the DK1/DK2 view rotation (set this up as you need it)
			if( arg == "--rotate-view" )
			{
				ROTATE_VIEW = true;
			}
			// This flag enables the program to run even without a physical device connected
			/*	REMOVED: Rift class is now able to detect if there is a compatible headset connected!
			if( arg == "--no-rift" )
			{
				NO_RIFT = true;
			}
			*/
			// This flag disables the debug window (simply a window where stereo virtual cameras are showed before going through Rift class)
			if( arg == "--no-debug" )
			{
				DEBUG_WINDOW = false;
			}
			if( arg == "--help" || arg == "-h" )
			{
				std::cout << "Available Commands:" << std::endl
					<< "\t--rotate-view\tChanges the orientation of the main render window. Useful when your computer can't rotate the screen." << std::endl
					<< "\t--no-rift\tFor debugging: disable the Oculus Rift." << std::endl
					<< "\t--no-debug\tDisables the debug window." << std::endl
					<< "\t--help,-h\tShow this help message." << std::endl;
				exit(0);	// show help and then close app.
			}
		}

		// Creates the main program and starts rendering. When a framelistener
		// returns false, this will return.
		App* app = new App();

		delete app;

        return 0;
    }
}
