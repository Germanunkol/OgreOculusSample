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

Rift::Rift( int ID )
{
	if( ! isInitialized ) throw( "Need to initialize first. Call Rift::init()!" );
	std::cout << "Creating Rift (ID: " << ID << ")" << std::endl;
	hmd = NULL;
	ovr_Initialize();

	hmd = ovrHmd_Create( ID );
    if( !hmd )
	{
		hmd = NULL;
		throw( "Could not connect to Rift." );
	}

	if (!ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0) )
	{
		ovrHmd_Destroy(hmd);
		throw ("\t\tThis Rift does not support the features needed by the application.");
	}

	/*hmd = NULL;
	
	if ( ID > ovrHmd_Detect() )
	{
		std::cout << "\tRift with ID " << ID << " requested, but only " << ovrHmd_Detect() << " devices available." << std::endl;
		return;
	}

	mRiftID = ID;

	hmd = ovrHmd_Create( mRiftID );

	throw( "Could not connect to Oculus Rift." );

	std::cout << "\tFound and initialized Rift." << std::endl;

	ovrHmd_GetDesc(hmd, &hmdDesc);

	std::cout << "\tProduct Name: " <<  hmdDesc.ProductName << std::endl;
	std::cout << "\tManufacturer " <<  hmdDesc.Manufacturer << std::endl;
	std::cout << "\tHmdCaps " <<  hmdDesc.HmdCaps << std::endl;
	std::cout << "\tSensorCaps " <<  hmdDesc.SensorCaps << std::endl;
	std::cout << "\tDistortionCaps " <<  hmdDesc.DistortionCaps << std::endl;
	//std::cout << "\tResolution " <<  hmdDesc.Resolution << std::endl;

	if( !ovrHmd_StartSensor(hmd, ovrSensorCap_Orientation | ovrSensorCap_YawCorrection, ovrSensorCap_Orientation) )
	{
		ovrHmd_Destroy(hmd);
		throw ("\t\tThis Rift does not support the features needed by the application.");
	}*/
}

Rift::~Rift()
{
	if(hmd) ovrHmd_Destroy(hmd);
	ovr_Shutdown();
}

bool Rift::update()
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
		std::cout << "yaw: "   << RadToDegree(yaw)   << std::endl;
		std::cout << "pitch: " << RadToDegree(pitch) << std::endl;
		std::cout << "roll: "  << RadToDegree(roll)  << std::endl;
		mOrientation = Ogre::Quaternion( pose.Rotation.w,
			pose.Rotation.x, pose.Rotation.y, pose.Rotation.z );
	}

	ovrHmd_EndFrameTiming(hmd);

	return true;
}
