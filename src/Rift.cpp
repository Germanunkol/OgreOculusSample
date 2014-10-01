#include "Rift.h"

//////////////////////////////////////////
// Static members for handling the API:
//////////////////////////////////////////
void Rift::init()
{
	ovr_Initialize();
}
void Rift::shutdown()
{
	ovr_Shutdown();
}

/////////////////////////////////////////
// Per-Device methods (non static):
/////////////////////////////////////////

Rift::Rift( int ID )
{
	hmd = NULL;
	std::cout << "Creating Rift (ID: " << ID << ")" << std::endl;
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
}

bool Rift::update()
{
	/*frameTiming = ovrHmd_BeginFrameTiming(hmd, 0); 
	ovrSensorState ss = ovrHmd_GetSensorState(hmd, frameTiming.ScanoutMidpointSeconds);

	if(ss.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)){
		ovrPosef pose = ss.Predicted.Pose;

		mOrientation.x = pose.Orientation.x;
		mOrientation.y = pose.Orientation.y;
		mOrientation.z = pose.Orientation.z;
		mOrientation.w = pose.Orientation.w;

		ovrHmd_EndFrameTiming(hmd);

		//if (_kbhit()) exit(0);
	}*/
	return true;
}
