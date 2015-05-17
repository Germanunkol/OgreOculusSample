#ifndef RIFT_H
#define RIFT_H

#include <iostream>
#include "OVR.h"
#include "Extras\OVR_Math.h"
#include "OGRE/Ogre.h"
using namespace OVR;

class Rift
{
	public:
		// Contructor initiates Rift Rendering environment, as follows:
		//	- looks for Rift physical device (it can be FOUND or NOT FOUND)
		//	- create a new scene under root with:
		//		- two distorsion meshes so that render results can be displayed on their textures
		//		- an ortho camera looking at the meshes, so final render can be displayed into Rift
		//	- creates a window to render the ortho camera:
		//		- IF renderWindow == nullptr
		//			- if Rift is FOUND, creates a full screen window in the secondary screen (default: id=1)
		//			- if Rift is NOT FOUND, creates a window in the default screen
		//		- IF renderWindow != nullptr, result of ortho camera is displayed regardless of Oculus state
		//	- rotates the camera/meshes by 90 degrees if rotateView == true (DK2 Setup)
		Rift( const unsigned int ID, Ogre::Root* const root, Ogre::RenderWindow* &renderWindow, const bool rotateView = false);
		~Rift();

		// Call this if you want to manually associate a render window to the Rift
		// Current used window will be replaced
		//void setWindow(Ogre::Root* root, Ogre::RenderWindow* renderWindow, bool rotateView);

		// MUST BE CALLED to set up viewports correctly
		// Create the cameras in your scene, then call this function
		// The two input cameras will be rendered on the two distortion meshes
		void setCameras( Ogre::Camera* camLeft, Ogre::Camera* camRight );

		// Update Rift data every frame. This should return true as long as data is read from rift.
		bool update( float dt );

		Ogre::Quaternion getOrientation() { return mOrientation; }
		Ogre::Vector3 getPosition() { return mPosition; }

		// returns interpupillary distance in meters: (Default: 0.064m)
		float getIPD() { return mIPD; }

		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }

		// Used to reset head position and orientation to "foreward".
		// Call this when user presses 'R', for example.
		void recenterPose();

		/*
		well why be an arist if the only thing you could give is the way you express it?

		*/

	private:

		// Rift data
		int mRiftID;
		ovrHmd hmd;
		bool simulationMode;
		float mIPD;
		ovrFrameTiming frameTiming;
		static bool isInitialized;
		static unsigned short int ovr_Users;
		static void init();
		static void shutdown();

		// Oculus Rift Outer Scene representation (virtual stereo cameras + head + body)
		Ogre::Vector3 mPosition;
		Ogre::Quaternion mOrientation;
		Ogre::Camera* mCamLeft;
		Ogre::Camera* mCamRight;
		Ogre::SceneNode* mHeadNode;
		Ogre::SceneNode* mBodyNode;
		Ogre::SceneNode* mBodyTiltNode;
		// Oculus Rift Inner Scene representation (barrell effect for Oculus display)
		Ogre::SceneManager* mSceneMgr;
			Ogre::SceneNode* mCamNode;
				Ogre::Camera* mCamera;
			Ogre::TexturePtr mLeftEyeRenderTexture;
			Ogre::TexturePtr mRightEyeRenderTexture;
			Ogre::MaterialPtr mMatLeft;
			Ogre::MaterialPtr mMatRight;
		void createRiftDisplayScene(Ogre::Root* const root, const bool rotateView);
		// Oculus Rift Display rendering window (displayed on Oculus)
		Ogre::RenderWindow* mRenderWindow;
		Ogre::Viewport* mViewport;
		void createRiftDisplayWindow(Ogre::Root* const root);














		//Ogre::SceneNode* mRoomNode;
		
		


	
};

#endif
