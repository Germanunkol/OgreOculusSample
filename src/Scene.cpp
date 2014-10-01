#include "Scene.h"

Scene::Scene( Ogre::Root* root, OIS::Mouse* mouse, OIS::Keyboard* keyboard )
{
	mRoot = root;
	mMouse = mouse;
	mKeyboard = keyboard;
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC); 

	// Set up main Lighting and Shadows in Scene:
	mSceneMgr->setAmbientLight( Ogre::ColourValue(0.1f,0.1f,0.1f) );
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	mSceneMgr->setShadowFarDistance( 30 );

	createRoom();
	createCameras();
}

Scene::~Scene()
{
	if (mSceneMgr) delete mSceneMgr;
}

void Scene::createRoom()
{
	mRoomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RoomNode");

	Ogre::SceneNode* cubeNode = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt = mSceneMgr->createEntity( "Cube.mesh" );
	cubeNode->attachObject( cubeEnt );
	cubeNode->setPosition( 1.0, 0.0, 0.0 );
	Ogre::SceneNode* cubeNode2 = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt2 = mSceneMgr->createEntity( "Cube2.mesh" );
	cubeNode2->attachObject( cubeEnt2 );
	cubeNode2->setPosition( -1.0, 0.0, 0.0 );

	Ogre::Entity* roomEnt = mSceneMgr->createEntity( "Room.mesh" );
	mRoomNode->attachObject( roomEnt );

	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows( true );
	roomLight->setShadowFarDistance( 30 );
	roomLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	roomLight->setSpecularColour( .25, .25, .25 );
	roomLight->setDiffuseColour( 0.35, 0.27, 0.23 );

	roomLight->setPosition( 5, 5, 5 );

	mRoomNode->attachObject( roomLight );
}

void Scene::createCameras()
{
	mCamLeft = mSceneMgr->createCamera("LeftCamera");
	mCamRight = mSceneMgr->createCamera("RightCamera");

	// Create a scene nodes which the cams will be attached to:
	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BodyNode");
	mHeadNode = mBodyNode->createChildSceneNode("HeadNode"); 
	mBodyNode->setFixedYawAxis( true );	// don't roll!  

	mHeadNode->attachObject(mCamLeft);
	mHeadNode->attachObject(mCamRight);

	// Position cameras according to interpupillary distance
	float dist = 0.05;
	/*if (mRift->isAttached())
	{
		dist = mRift->getStereoConfig().GetIPD();
	}*/
	mCamLeft->setPosition( -dist/2.0f, 0.0f, 0.0f );
	mCamRight->setPosition( dist/2.0f, 0.0f, 0.0f );

	// If possible, get the camera projection matricies given by the rift:	
	/*if (mRift->isAttached())
	{
		mCamLeft->setCustomProjectionMatrix( true, mRift->getProjectionMatrix_Left() );
		mCamRight->setCustomProjectionMatrix( true, mRift->getProjectionMatrix_Right() );
	}*/
	mCamLeft->setFarClipDistance( 50 );
	mCamRight->setFarClipDistance( 50 );

	mCamLeft->setNearClipDistance( 0.001 );
	mCamRight->setNearClipDistance( 0.001 );

	/*mHeadLight = mSceneMgr->createLight();
	mHeadLight->setType(Ogre::Light::LT_POINT);
	mHeadLight->setCastShadows( true );
	mHeadLight->setShadowFarDistance( 30 );
	mHeadLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	mHeadLight->setSpecularColour( 1.0, 1.0, 1.0 );
	mHeadLight->setDiffuseColour( 1.0, 1.0, 1.0 );
	mHeadNode->attachObject( mHeadLight );*/

	mBodyNode->setPosition( 2.0, 0.5, 2.0 );
	mBodyNode->lookAt( Ogre::Vector3::ZERO, Ogre::SceneNode::TS_WORLD );

	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65, 1.0, 0.07, 0.017 );
	light->setSpecularColour( .25, .25, .25 );
	light->setDiffuseColour( 0.35, 0.27, 0.23 );
	mBodyNode->attachObject(light);
}

void Scene::update( float dt )
{
}

//////////////////////////////////////////////////////////////
// Handle Rift Input:
//////////////////////////////////////////////////////////////

void Scene::setRiftPose( Ogre::Quaternion orientation )
{
	mHeadNode->setOrientation( orientation );
}

//////////////////////////////////////////////////////////////
// Handle Input:
//////////////////////////////////////////////////////////////

bool Scene::keyPressed( const OIS::KeyEvent& e )
{
	return true;
}
bool Scene::keyReleased( const OIS::KeyEvent& e )
{
	return true;
}
bool Scene::mouseMoved( const OIS::MouseEvent& e )
{
	if( mMouse->getMouseState().buttonDown( OIS::MB_Left ) )
	{
		mBodyNode->yaw( Ogre::Degree( e.state.X.rel ) );
		mBodyNode->pitch( Ogre::Degree( e.state.Y.rel ) );
	}
	return true;
}
bool Scene::mousePressed( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}
bool Scene::mouseReleased( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}