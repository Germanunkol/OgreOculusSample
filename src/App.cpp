#include "App.h"

App::App()
{
	std::cout << "Creating Ogre application:" << std::endl;

	mRoot = NULL;
	mKeyboard = NULL;
	mMouse = NULL;	
	mScene = NULL;
	mShutdown = false;
	mWindow = NULL;
	mSmallWindow = NULL;
	mRift = NULL;

	initOgre();
	initOIS();

	mScene = new Scene( mRoot, mMouse, mKeyboard );

	createViewports();

	initRift();

	//Ogre::WindowEventUtilities::messagePump();

	mRoot->startRendering();
}

App::~App()
{
	std::cout << "Deleting Ogre application." << std::endl;

	quitRift();

	std::cout << "Deleting Scene:" << std::endl;
	if( mScene ) delete mScene;

	std::cout << "Closing OIS:" << std::endl;
	quitOIS();
	std::cout << "Closing Ogre:" << std::endl;
	quitOgre();
}

////////////////////////////////////////////////////////////
// Handle Ogre (construction and destruction of ogre root):
////////////////////////////////////////////////////////////

void App::initOgre()
{

	try
	{
		mRoot = new Ogre::Root("cfg/plugins.cfg", "%TEMP%/ogre.cfg", "%TEMP%/ogre.log");
	}
	catch ( Ogre::FileNotFoundException &e )
	{
		try
		{
#ifdef _DEBUG
			mRoot = new Ogre::Root("../../cfg/plugins_d.cfg", "%TEMP%/ogre.cfg", "%TEMP%/ogre.log");
#else
			mRoot = new Ogre::Root("../../cfg/plugins.cfg", "%TEMP%/ogre.cfg", "%TEMP%/ogre.log");
#endif
		}
		catch ( Ogre::FileNotFoundException &e )
		{
			throw e;
		}
	}

	mRoot->addFrameListener(this);

	// Load up resources according to resources.cfg:
	Ogre::ConfigFile cf;
    cf.load("resources.cfg");
 
    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey(); Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
	}

	Ogre::RenderSystem* pRS = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	Ogre::ConfigOptionMap cfgMap = pRS->getConfigOptions();
	cfgMap["Full Screen"].currentValue = "No";
	cfgMap["VSync"].currentValue = "Yes";
	#ifdef _DEBUG
		cfgMap["FSAA"].currentValue = "0";
	#else
		cfgMap["FSAA"].currentValue = "8";
	#endif
	cfgMap["Video Mode"].currentValue = "1200 x 800";
	for(Ogre::ConfigOptionMap::iterator iter = cfgMap.begin(); iter != cfgMap.end(); iter++) pRS->setConfigOption(iter->first, iter->second.currentValue);
	mRoot->setRenderSystem(pRS);
	mRoot->initialise(false, "Oculus Rift Visualization");

	// Create the Windows:
	Ogre::NameValuePairList miscParams;
	if( NO_RIFT )
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(0);
	else
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(1);
	miscParams["border "] = "none";
	
	Ogre::NameValuePairList miscParamsSmall;
	miscParamsSmall["monitorIndex"] = Ogre::StringConverter::toString(0);

	if( !ROTATE_VIEW )
		mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1920, 1080, true, &miscParams);
		//mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1920*0.5, 1080*0.5, false, &miscParams);
	else
		mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1080, 1920, true, &miscParams);

	if( DEBUG_WINDOW )
		mSmallWindow = mRoot->createRenderWindow("DEBUG Oculus Rift Liver Visualization", 1920*debugWindowSize, 1080*debugWindowSize, false, &miscParamsSmall);   

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void App::quitOgre()
{
	// This should really not be commented out, but it crashes on my system... ?!
	// if( mRoot ) delete mRoot;
}

////////////////////////////////////////////////////////////
// Handle OIS (construction and destruction of input):
////////////////////////////////////////////////////////////

void App::initOIS()
{
	OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
 
    //tell OIS about the Ogre window
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	//setup the manager, keyboard and mouse to handle input
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
 
    //tell OIS about the window's dimensions
    unsigned int width, height, depth;
    int top, left;
    mWindow->getMetrics(width, height, depth, left, top);
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

	// Make sure OIS calls callbacks (keyPressed, mouseMoved etc) of this class:
	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);
}

void App::quitOIS()
{
	if( mKeyboard ) delete mKeyboard;
	if( mMouse ) delete mMouse;
}

////////////////////////////////////////////////////////////
// Handle Rift Devices:
////////////////////////////////////////////////////////////

void App::initRift()
{
	// Try to initialize the Oculus Rift (ID 0):
	try {
		Rift::init();
		mRift = new Rift( 0, mRoot, mWindow, ROTATE_VIEW );
		mRift->setCameras( mScene->getLeftCamera(), mScene->getRightCamera() );
		mScene->setIPD( mRift->getIPD() );
	} catch( const char* e ) {
		std::cout << ">> " << e << std::endl;
		mRift = NULL;
		mShutdown = true;
	}
}

void App::quitRift()
{
	std::cout << "Shutting down Oculus Rifts:" << std::endl;
	if( mRift ) delete mRift;
	Rift::shutdown();
}

////////////////////////////////////////////////////////////
// Handle Rendering:
////////////////////////////////////////////////////////////

void App::createViewports()
{
	// Create two viewports, one for each eye (i.e. one for each camera):
	// Each viewport spans half of the screen
	if(mWindow)
	{
		if( NO_RIFT )
		{
			mViewportL = mWindow->addViewport(mScene->getLeftCamera(), 0, 0.0, 0.0, 0.5, 1.0 );
			mViewportL->setBackgroundColour(Ogre::ColourValue(0.15,0.15,0.15));
			mViewportR = mWindow->addViewport(mScene->getRightCamera(), 1, 0.5, 0.0, 0.5, 1.0 );
			mViewportR->setBackgroundColour(Ogre::ColourValue(0.15,0.15,0.15));
		}
		
		/*if( !ROTATE_VIEW )
		{
			mScene->getLeftCamera()->setAspectRatio( 0.5*mWindow->getWidth()/mWindow->getHeight() );
			mScene->getRightCamera()->setAspectRatio( 0.5*mWindow->getWidth()/mWindow->getHeight() );
		} else {
			mScene->getLeftCamera()->setAspectRatio( 0.5*mWindow->getHeight()/mWindow->getWidth() );
			mScene->getRightCamera()->setAspectRatio( 0.5*mWindow->getHeight()/mWindow->getWidth() );
		}*/
	}

	if( mSmallWindow )
	{
		Ogre::Viewport* debugL = mSmallWindow->addViewport(mScene->getLeftCamera(), 0, 0.0, 0.0, 0.5, 1.0 );
		debugL->setBackgroundColour(Ogre::ColourValue(0.15,0.15,0.15));

		Ogre::Viewport* debugR = mSmallWindow->addViewport(mScene->getRightCamera(), 1, 0.5, 0.0, 0.5, 1.0 );
		debugR->setBackgroundColour(Ogre::ColourValue(0.15,0.15,0.15));
	}
}

bool App::frameRenderingQueued(const Ogre::FrameEvent& evt) 
{
	if (mShutdown) return false;

	if(mRift)
	{
		if ( mRift->update( evt.timeSinceLastFrame ) )
		{
			mScene->setRiftPose( mRift->getOrientation(), mRift->getPosition() );
		} else {
			delete mRift;
			mRift = NULL;
		}
	}

	//update the standard input devices
	mKeyboard->capture();
	mMouse->capture();

	mScene->update( evt.timeSinceLastFrame );

	//exit if key ESCAPE pressed 
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) 
		return false;

	return true; 
}

//////////////////////////////////////////////////////////////////////////
// Handle Keyboard and Mouse input (and pass it on to the User Interface)
//////////////////////////////////////////////////////////////////////////

bool App::keyPressed( const OIS::KeyEvent& e )
{
	mScene->keyPressed( e );

	if( e.key == OIS::KC_P )
		mWindow->writeContentsToFile("Screenshot.png");

	return true;
}
bool App::keyReleased( const OIS::KeyEvent& e )
{
	mScene->keyReleased( e );
	return true;
}
bool App::mouseMoved( const OIS::MouseEvent& e )
{
	mScene->mouseMoved( e );
	return true;
}
bool App::mousePressed( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	mScene->mouseReleased( e, id );
	return true;
}
bool App::mouseReleased( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	mScene->mouseReleased( e, id );
	return true;
}

void App::quit()
{
	std::cout << "QUIT." << std::endl;
	mShutdown = true;
}
