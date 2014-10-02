#ifndef RIFT_H
#define RIFT_H

#include <iostream>

#include "OVR.h"
#include "Kernel/OVR_Math.h"
#include "OGRE/Ogre.h"
using namespace OVR;

class Rift
{
	public:
		Rift( int ID, Ogre::Root* root, Ogre::RenderWindow* renderWindow );
		~Rift();
		
		static void init();
		static void shutdown();

		void setCameras( Ogre::Camera* camLeft, Ogre::Camera* camRight );

		// Update Rift data every frame. This should return true as long as data is read from rift.
		bool update( float dt );

		Ogre::Quaternion getOrientation() { return mOrientation; }
		
		static bool isInitialized;

	private:

		ovrHmd hmd;
		ovrFrameTiming frameTiming;

		int mRiftID;

		Ogre::Quaternion mOrientation;
		
		Ogre::TexturePtr mLeftEyeRenderTexture;
		Ogre::TexturePtr mRightEyeRenderTexture;

		Ogre::SceneManager* mSceneMgr;
		Ogre::RenderWindow* mRenderWindow;

		Ogre::Camera* mCamera;
		Ogre::Viewport* mViewport;
		Ogre::SceneNode* mCamNode;

		Ogre::SceneNode* mRoomNode;
		Ogre::Camera* mCamLeft;
		Ogre::Camera* mCamRight;

		Ogre::SceneNode* mHeadNode;
		Ogre::SceneNode* mBodyNode;
		Ogre::SceneNode* mBodyTiltNode;
};

#endif
