#include "Rift.h"

bool Rift::isInitialized = false;

//////////////////////////////////////////
// Static members for handling the API:
//////////////////////////////////////////
void Rift::init()
{
	if( ! isInitialized )
	{
		ovr_Initialize();
		isInitialized = true;
	}
}
void Rift::shutdown()
{
	if( isInitialized )
	{
		ovr_Shutdown();
		isInitialized = false;
	}
}

/////////////////////////////////////////
// Per-Device methods (non static):
/////////////////////////////////////////

Rift::Rift( int ID, Ogre::Root* root, Ogre::RenderWindow* renderWindow )
{
	if( ! isInitialized ) throw( "Need to initialize first. Call Rift::init()!" );
	std::cout << "Creating Rift (ID: " << ID << ")" << std::endl;

	mSceneMgr = root->createSceneManager(Ogre::ST_GENERIC);
	mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.5, 0.5, 0.5 ) );

	mRenderWindow = renderWindow;

	hmd = NULL;

	hmd = ovrHmd_Create( ID );
    if( !hmd )
	{
		hmd = NULL;
		throw( "Could not connect to Rift." );
	}

	std::cout << "Oculus Rift found." << std::endl;
	std::cout << "\tProduct Name: " << hmd->ProductName << std::endl;
	std::cout << "\tProduct ID: " << hmd->ProductId << std::endl;
	std::cout << "\tFirmware: " << hmd->FirmwareMajor << "." << hmd->FirmwareMinor << std::endl;
	std::cout << "\tResolution: " << hmd->Resolution.w << "x" << hmd->Resolution.h << std::endl;

	if (!ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0) )
	{
		ovrHmd_Destroy(hmd);
		throw ("\t\tThis Rift does not support the features needed by the application.");
	}

	// Configure Render Textures:
	Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize( hmd, ovrEye_Left,
		hmd->DefaultEyeFov[0], 1.0f );
	Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize( hmd, ovrEye_Right,
		hmd->DefaultEyeFov[1], 1.0f );

	/*Sizei renderTargetSize;
	renderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
	renderTargetSize.h = std::max( recommendedTex0Size.h, recommendedTex1Size.h );*/

	mLeftEyeRenderTexture = Ogre::TextureManager::getSingleton().createManual(
		"RiftRenderTextureLeft", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, recommendedTex0Size.w, recommendedTex0Size.h, 0, Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET );
	mRightEyeRenderTexture = Ogre::TextureManager::getSingleton().createManual(
		"RiftRenderTextureRight", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, recommendedTex1Size.w, recommendedTex1Size.h, 0, Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET );

	// Assign the textures to the eyes used later:
	Ogre::MaterialPtr matLeft = Ogre::MaterialManager::getSingleton().getByName( "Oculus/LeftEye" );
	matLeft->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture( mLeftEyeRenderTexture );
	Ogre::MaterialPtr matRight = Ogre::MaterialManager::getSingleton().getByName( "Oculus/RightEye" );
	matRight->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture( mRightEyeRenderTexture );

	ovrEyeRenderDesc eyeRenderDesc[2];
	
	eyeRenderDesc[0] = ovrHmd_GetRenderDesc( hmd, ovrEye_Left, hmd->DefaultEyeFov[0] );
	eyeRenderDesc[1] = ovrHmd_GetRenderDesc( hmd, ovrEye_Right, hmd->DefaultEyeFov[1] );
	
	std::cout << eyeRenderDesc[0].Fov.DownTan << std::endl;
	std::cout << eyeRenderDesc[0].Eye << std::endl;

	ovrVector2f UVScaleOffset[2];
	ovrRecti viewports[2];
	viewports[0].Pos.x = 0;
	viewports[0].Pos.y = 0;
	viewports[0].Size.w = recommendedTex0Size.w;
	viewports[0].Size.h = recommendedTex0Size.h;
	viewports[1].Pos.x = 0;
	viewports[1].Pos.y = recommendedTex0Size.w;
	viewports[1].Size.w = recommendedTex1Size.w;
	viewports[1].Size.h = recommendedTex1Size.h;

	Ogre::SceneNode* meshNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	// Create the Distortion Meshes:
	for ( int eyeNum = 0; eyeNum < 2; eyeNum ++ )
	{
		ovrDistortionMesh meshData;
		ovrHmd_CreateDistortionMesh( hmd,
			eyeRenderDesc[eyeNum].Eye,
			eyeRenderDesc[eyeNum].Fov,
			0,
			&meshData );

		/*ovrHmd_GetRenderScaleAndOffset( eyeRenderDesc[eyeNum].Fov,
			renderTargetSize, viewports[eyeNum],
			&UVScaleOffset[eyeNum]);*/

		// create ManualObject
		// TODO: Destroy the manual objects!!
		Ogre::ManualObject* manual;
		if( eyeNum == 0 )
		{
			manual = mSceneMgr->createManualObject("RiftRenderObjectLeft");
			manual->begin("Oculus/LeftEye", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		}
		else
		{
			manual = mSceneMgr->createManualObject("RiftRenderObjectRight");
			manual->begin("Oculus/RightEye", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		}
 
		// specify the material (by name) and rendering type
 
		for( unsigned int i = 0; i < meshData.VertexCount; i++ )
		{
			manual->position( meshData.pVertexData[i].ScreenPosNDC.x,
				meshData.pVertexData[i].ScreenPosNDC.y, 0 );
			//manual->textureCoord( meshData.pVertexData[i].TanEyeAnglesG.x,
			//	meshData.pVertexData[i].TanEyeAnglesG.y);
			manual->textureCoord( meshData.pVertexData[i].TanEyeAnglesG.x/2 + 0.5,
				meshData.pVertexData[i].TanEyeAnglesG.y/2 + 0.5);
		}
		for( unsigned int i = 0; i < meshData.IndexCount; i++ )
		{
			manual->index( meshData.pIndexData[i] );
		}
 
		// tell Ogre, your definition has finished
		manual->end();

		ovrHmd_DestroyDistortionMesh( &meshData );

		meshNode->attachObject( manual );
	}

	// Create a camera in the (new, external) scene so the mesh can be rendered onto it:
	mCamera = mSceneMgr->createCamera("OculusRiftExternalCamera");
	mCamera->setFarClipDistance( 50 );
	mCamera->setNearClipDistance( 0.001 );

	mSceneMgr->getRootSceneNode()->attachObject( mCamera );

	meshNode->setPosition( 0, 0, -1 );
	meshNode->setScale( 1, 1, -1 );

	mViewport = mRenderWindow->addViewport( mCamera );
	mViewport->setBackgroundColour(Ogre::ColourValue::Black);
}

Rift::~Rift()
{
	if(hmd) ovrHmd_Destroy(hmd);
}

// Takes the two cameras created in the scene and creates Viewports in the correct render textures:
void Rift::setCameras( Ogre::Camera* camLeft, Ogre::Camera* camRight )
{
	Ogre::RenderTexture* renderTexture = mLeftEyeRenderTexture->getBuffer()->getRenderTarget();
	renderTexture->addViewport(camLeft);
	renderTexture->getViewport(0)->setClearEveryFrame(true);
	renderTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
	renderTexture->getViewport(0)->setOverlaysEnabled(false);

	renderTexture = mRightEyeRenderTexture->getBuffer()->getRenderTarget();
	renderTexture->addViewport(camRight);
	renderTexture->getViewport(0)->setClearEveryFrame(true);
	renderTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
	renderTexture->getViewport(0)->setOverlaysEnabled(false);
}

bool Rift::update( float dt )
{
	if( !hmd ) return true;

	frameTiming = ovrHmd_BeginFrameTiming(hmd, 0);
	ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, frameTiming.ScanoutMidpointSeconds);

	if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
		// The cpp compatibility layer is used to convert ovrPosef to Posef (see OVR_Math.h)
		Posef pose = ts.HeadPose.ThePose;
		float yaw, pitch, roll;
		pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

		// Optional: move cursor back to starting position and print values
		//std::cout << "yaw: "   << RadToDegree(yaw)   << std::endl;
		//std::cout << "pitch: " << RadToDegree(pitch) << std::endl;
		//std::cout << "roll: "  << RadToDegree(roll)  << std::endl;
		mOrientation = Ogre::Quaternion( pose.Rotation.w,
			pose.Rotation.x, pose.Rotation.y, pose.Rotation.z );
	}

	ovrHmd_EndFrameTiming(hmd);

	return true;
}
