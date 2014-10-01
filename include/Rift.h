#ifndef RIFT_H
#define RIFT_H

#include <iostream>

#include "OVR.h"
//#include "Kernel/OVR_Math.h"
#include "OGRE/Ogre.h"
using namespace OVR;

class Rift
{
	public:
		Rift( int ID );
		~Rift();
		
		static void init();
		static void shutdown();

		// Update Rift data every frame. This should return true as long as data is read from rift.
		bool update();

		Ogre::Quaternion getOrientation() { return mOrientation; }

	private:

		ovrHmd hmd;
		ovrHmdDesc hmdDesc;
		ovrFrameTiming frameTiming;

		int mRiftID;

		Ogre::Quaternion mOrientation;
};

#endif
