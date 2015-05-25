/*! \class Rift
 * \brief A class which handles interaction with the Oculus %Rift.
 *
 * This class acts as an interface between the Ogre Scene and the Oculus %Rift.
 * To display content on the HMD, this class generates a new scene (Rift-Scene) and places a distortion mesh
 * into the scene. Then it generates two RenderTextures and lets the in-scene cameras render to
 * these textures, which are placed onto the distortion mesh.
 * 
 * A new (orthogonal) camera is placed in the Rift-Scene to render the distortion mesh.
 *
 * Note: In the future, the Rift-Scene may be replaced by an Ogre compositor, once mesh rendering
 * in compositors is in place (Ogre 2.0 onward).
 */


#ifndef RIFT_H
#define RIFT_H

#include <iostream>

#include "OVR.h"
#include "Kernel/OVR_Math.h"
#include "OGRE/Ogre.h"
#include "OGRE/OgreSingleton.h"
using namespace OVR;

class Rift : public Ogre::Singleton<Rift>
{
	public:
		//! Constructor. By generating a new Rift instance, you automatically connect to the Rift.
		/*! Make sure to call init() before you create an instance of this class.
		 */
		Rift( int ID, Ogre::Root* root, Ogre::RenderWindow* renderWindow, bool rotateView );

		//! Destructor.
		/*! Should clean all resourcs. TODO!
		 */
		~Rift();
	
		//! Must be called before any Rift instance is created.
		static void init();
		//! Call when program is about to shut down and after last Rift instance has been deleted.
		static void shutdown();
		
		//! Create the cameras in your scene, then call this function
		/*! Must be called to set up viewports correctly.
		 * This connects the images on the screen with the in-scene cameras.
		 */
		void setCameras( Ogre::Camera* camLeft, Ogre::Camera* camRight );

		//! Update Rift data every frame. Returns true as long as data is read from rift.
		bool update( float dt );

		//! Get the last reported orientation from the Rift.
		Ogre::Quaternion getOrientation() { return mOrientation; }
		//! Get the last reported position from the Rift.
		Ogre::Vector3 getPosition() { return mPosition; }

		//! returns interpupillary distance in meters: (Default: 0.064m)
		float getIPD() { return mIPD; }
	
		//! Returns the scene manager of the second scene which the Rift class creates.
		/*! This class creates a new Ogre::SceneManager (which can be retrieved using this
		 * function) to render the distortion mesh in.
		 */
		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }

		//! Used to reset head position and orientation to "foreward".
		/*! Call this when user presses 'R', for example. */
		void recenterPose();

		//! Use to set a custom texture (for example: loading screen/menu etc) on the rift.
		/*! Pass the name of an already loaded texture.*/
		void setTexture( std::string tex );

		//! Find out if this is a dummy rift only.
		/*! This will return to true if Rift wasn't found at startup.
		 */
		bool isDummyRift() { return mUseDummyHMD; }

	protected:

		//! Is true if the Rift::init function has successfully initialized the rift SDK.
		static bool isInitialized;


	private:

		ovrHmd hmd;
		ovrFrameTiming frameTiming;

		int mRiftID;

		Ogre::Quaternion mOrientation;
		Ogre::Vector3 mPosition;
		
		Ogre::TexturePtr mLeftEyeRenderTexture;
		Ogre::TexturePtr mRightEyeRenderTexture;

		Ogre::SceneManager* mSceneMgr;
		Ogre::RenderWindow* mRenderWindow;

		Ogre::Camera* mCamera;
		Ogre::Viewport* mViewport;
		Ogre::SceneNode* mCamNode;

		Ogre::SceneNode* mHeadNode;
		Ogre::SceneNode* mBodyNode;
		Ogre::SceneNode* mBodyTiltNode;
		
		Ogre::MaterialPtr mMatLeft;
		Ogre::MaterialPtr mMatRight;

		//! The distance between the eyes.
		float mIPD;

		//! This will be true if the rift is not found and we should instead use a dummy.
		bool mUseDummyHMD;

		Ogre::Viewport* mLeftDummyViewport;
		Ogre::Viewport* mRightDummyViewport;
};

#endif
