#include "App.h"
#include <chrono>

void frames_per_second(int delay)
{
	static int num_frame = 0;
	static int time = 0;

	num_frame++;
	time += delay;
	if (time >= 1000)
	{
		std::cout << "fps:" << num_frame << std::endl;
		num_frame = 0;
		time = 0;
	}

}
std::chrono::steady_clock::time_point last_request;
std::chrono::duration< int, std::milli > delay;

////////////////////////////////////////////////////////////
// Init application
////////////////////////////////////////////////////////////

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

	//Ogre engine setup (creates Ogre main rendering window)
	initOgre();

	//Rift Setup (creates Oculus rendering window and Oculus inner scene - user shouldn't care about it)
	initRift();

	//Input/Output setup (associate I/O to Oculus window)
	initOIS();

	// Create Ogre main scene (setup and populate main scene)
	// This class implements App logic!!
	mScene = new Scene(mRoot, mMouse, mKeyboard);
	mScene->setIPD(mRift->getIPD());

	//Viewport setup (link scene cameras to Ogre/Oculus windows)
	createViewports();

	//Ogre::WindowEventUtilities::messagePump();

	// START RENDERING!
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

/////////////////////////////////////////////////////////////////
// Init Ogre (construction, setup and destruction of ogre root):
/////////////////////////////////////////////////////////////////

void App::initOgre()
{
	// Config file class is an utility that parses and stores values from a .cfg file
	Ogre::ConfigFile cf;
	std::string configFilePathPrefix = "cfg/";			// configuration files default location when app is installed
#ifdef _DEBUG
	std::string pluginsFileName = "plugins_d.cfg";		// plugins config file name (Debug mode)
#else
	std::string pluginsFileName = "plugins.cfg";		// plugins config file name (Release mode)
#endif
	std::string resourcesFileName = "resources.cfg";	// resources config file name (Debug/Release mode)


	// LOAD OGRE PLUGINS
	// Try to load load up a valid config file (and don't start the program if none is found)
	try
	{
		//This will work ONLY when application is installed (only Release application)!
		cf.load(configFilePathPrefix + pluginsFileName);
	}
	catch (Ogre::FileNotFoundException &e)
	{
		try
		{
			// if no existing config, or could not restore it, try to load from a different location
			configFilePathPrefix = "../cfg/";

			//This will work ONLY when application is in development (Debug/Release configuration)
			cf.load(configFilePathPrefix + pluginsFileName);			
		}
		catch (Ogre::FileNotFoundException &e)
		{
			// launch exception if no valid config file is found! - PROGRAM WON'T START!
			throw e;
		}
	}


	// INSTANCIATE OGRE ROOT (IT INSTANCIATES ALSO ALL OTHER OGRE COMPONENTS)
	// In Ogre, the singletons are instanciated explicitly (with new) the first time,
	// then it can be accessed with Ogre::Root::getSingleton()
	// Plugins are passed as argument to the "Root" constructor
	mRoot = new Ogre::Root(configFilePathPrefix + pluginsFileName, configFilePathPrefix + "ogre.cfg", "ogre.log");
	// No Ogre::FileNotFoundException is thrown by this, that's why we tried to open it first with ConfigFile::load()

	
	// LOAD OGRE RESOURCES
	// Load up resources according to resources.cfg ("cf" variable is reused)
	try
	{
		//This will work ONLY when application is installed!
		cf.load("cfg/resources.cfg");
	}
	catch (Ogre::FileNotFoundException &e)	// It works, no need to change anything
	{
		try
		{
			//This will work ONLY when application is in development (Debug/Release configuration)
			cf.load("../cfg/resources.cfg");
		}
		catch (Ogre::FileNotFoundException &e)
		{
			// launch exception if no valid config file is found! - PROGRAM WON'T START!
			throw e;
		}
	}

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
			//For each section/key-value, add a resource to ResourceGroupManager
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
	}


	// Then setup THIS CLASS INSTANCE as a frame listener
	// This means that Ogre will call frameStarted(), frameRenderingQueued() and frameEnded()
	// automatically and periodically if defined in this class
	mRoot->addFrameListener(this);


	// SELECT AND CUSTOMIZE OGRE RENDERING (OpenGL)
	// Get a reference of the RenderSystem in Ogre that I want to customize
	Ogre::RenderSystem* pRS = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	// Get current config RenderSystem options in a ConfigOptionMap
	Ogre::ConfigOptionMap cfgMap = pRS->getConfigOptions();
	// Modify them
	cfgMap["Full Screen"].currentValue = "No";
	cfgMap["VSync"].currentValue = "Yes";
	#ifdef _DEBUG
		cfgMap["FSAA"].currentValue = "0";
	#else
		cfgMap["FSAA"].currentValue = "8";
	#endif
	cfgMap["Video Mode"].currentValue = "1200 x 800";
	// Set them back into the RenderSystem
	for(Ogre::ConfigOptionMap::iterator iter = cfgMap.begin(); iter != cfgMap.end(); iter++) pRS->setConfigOption(iter->first, iter->second.currentValue);
	// Set this RenderSystem as the one I want to use
	mRoot->setRenderSystem(pRS);
	// Initialize it: "false" is DO NOT CREATE A WINDOW FOR ME
	mRoot->initialise(false, "Oculus Rift Visualization");


	// CREATE WINDOWS
	/* REMOVED: Rift class creates the window if no null is passed to its constructor
	// Options for Window 1 (rendering window)
	Ogre::NameValuePairList miscParams;
	if( NO_RIFT )
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(0);
	else
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(1);
	miscParams["border "] = "none";
	*/

	/*
	// Create Window 1
	if( !ROTATE_VIEW )
	mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1280, 800, true, &miscParams);
	//mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1920*0.5, 1080*0.5, false, &miscParams);
	else
	mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1080, 1920, true, &miscParams);
	*/

	// Options for Window 2 (debug window)
	// This window will simply show what the two cameras see in two different viewports
	Ogre::NameValuePairList miscParamsSmall;
	miscParamsSmall["monitorIndex"] = Ogre::StringConverter::toString(0);

	// Create Window 2
	if( DEBUG_WINDOW )
		mSmallWindow = mRoot->createRenderWindow("DEBUG Oculus Rift Liver Visualization", 1920*debugWindowSize, 1080*debugWindowSize, false, &miscParamsSmall);   

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void App::createViewports()
{
	// Create two viewports, one for each eye (i.e. one for each camera):
	// Each viewport is assigned to a RenderTarget (a RenderWindow in this case) and spans half of the screen
	// A pointer to a Viewport is returned, so we can access it directly.
	// CAMERA -> render into -> VIEWPORT (rectangle area) -> displayed into -> WINDOW
	/*
	if (mWindow)		//check if Ogre rendering window has been created
	{
		if (NO_RIFT)
		{
			mViewportL = mWindow->addViewport(mScene->getLeftCamera(), 0, 0.0, 0.0, 0.5, 1.0);
			mViewportL->setBackgroundColour(Ogre::ColourValue(0.15, 0.15, 0.15));
			mViewportR = mWindow->addViewport(mScene->getRightCamera(), 1, 0.5, 0.0, 0.5, 1.0);
			mViewportR->setBackgroundColour(Ogre::ColourValue(0.15, 0.15, 0.15));
		}

		if( !ROTATE_VIEW )
		{
		mScene->getLeftCamera()->setAspectRatio( 0.5*mWindow->getWidth()/mWindow->getHeight() );
		mScene->getRightCamera()->setAspectRatio( 0.5*mWindow->getWidth()/mWindow->getHeight() );
		} else {
		mScene->getLeftCamera()->setAspectRatio( 0.5*mWindow->getHeight()/mWindow->getWidth() );
		mScene->getRightCamera()->setAspectRatio( 0.5*mWindow->getHeight()/mWindow->getWidth() );
		/
	}
	*/

	// Plug the virtual stereo camera rig to Rift class (they will be rendered on Oculus screen)
	mRift->setCameras(mScene->getLeftCamera(), mScene->getRightCamera());

	// Create similar viewports to be displayed into PC window
	if (mSmallWindow)
	{
		Ogre::Viewport* debugL = mSmallWindow->addViewport(mScene->getLeftCamera(), 0, 0.0, 0.0, 0.5, 1.0);
		debugL->setBackgroundColour(Ogre::ColourValue(0.15, 0.15, 0.15));

		Ogre::Viewport* debugR = mSmallWindow->addViewport(mScene->getRightCamera(), 1, 0.5, 0.0, 0.5, 1.0);
		debugR->setBackgroundColour(Ogre::ColourValue(0.15, 0.15, 0.15));
	}
}

void App::quitOgre()
{
	// This should really not be commented out, but it crashes on my system... ?!
	// if( mRoot ) delete mRoot;
}

////////////////////////////////////////////////////////////
// Init OIS (construction, setup and destruction of input):
////////////////////////////////////////////////////////////

void App::initOIS()
{
	OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
 
    // Tell OIS about the Ogre Rendering window (give its id)
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	// Setup the manager, keyboard and mouse to handle input
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
 
    // Tell OIS about the window's dimensions
    unsigned int width, height, depth;
    int top, left;
    mWindow->getMetrics(width, height, depth, left, top);
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

	// Setup THIS CLASS INSTANCE as a OIS mouse listener AND key listener
	// This means that OIS will call keyPressed(), mouseMoved(), etc.
	// automatically and whenever needed
	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);
}

void App::quitOIS()
{
	if( mKeyboard ) delete mKeyboard;
	if( mMouse ) delete mMouse;
}

////////////////////////////////////////////////////////////////
// Init Rift (Device and API initialization, setup and close):
////////////////////////////////////////////////////////////////

void App::initRift()
{
	// Try to initialize the Oculus Rift (ID 0):
	try {
		// This class implements a custom C++ Class version of RIFT C API
		//Rift::init();		//OPTIONAL: automatically called by Rift constructor, if necessary
		mRift = new Rift( 0, mRoot, mWindow /*if null, Rift creates the window*/, ROTATE_VIEW );
	}
	catch (const std::ios_base::failure& e) {
		std::cout << ">> " << e.what() << std::endl;
		//NO_RIFT = true;
		mRift = NULL;
		mShutdown = true;
	}
}

void App::quitRift()
{
	std::cout << "Shutting down Oculus Rifts:" << std::endl;
	if( mRift ) delete mRift;
	//Rift::shutdown();
}

////////////////////////////////////////////////////////////
// Handle Rendering (Ogre::FrameListener)
////////////////////////////////////////////////////////////

// This gets called while rendering frame data is loading into GPU
// Good time to update measurements and physics before rendering next frame!
bool App::frameRenderingQueued(const Ogre::FrameEvent& evt) 
{
	//calculate delay and show
	delay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_request);
	frames_per_second(delay.count());

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

	//call updates on scene elements
	mScene->update( evt.timeSinceLastFrame );

	//get now time
	last_request = std::chrono::system_clock::now();

	//exit if key ESCAPE pressed 
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) 
		return false;

	return true; 
}

//////////////////////////////////////////////////////////////////////////////////
// Handle Keyboard and Mouse input (OIS::KeyListener, public OIS::MouseListener)
//////////////////////////////////////////////////////////////////////////////////

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

// Interrupts App loop
// frameRenderingQueued() will return false, thus interrupting Ogre loop
// App could run again by calling "mRoot->startRendering()"
// Destructor still needs to be called by user to deallocate application!
void App::quit()
{
	std::cout << "QUIT." << std::endl;
	mShutdown = true;
}
