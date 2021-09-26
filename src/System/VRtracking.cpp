#include "openvr.h"
#include "vr_support.h"
#include <iostream>
#include <cstring>


extern "C" double vrpos_hmdRotW = 0;
extern "C" double vrpos_hmdPitch = 0;
extern "C" double vrpos_hmdYaw = 0;
extern "C" double vrpos_hmdRoll = 0;

extern "C" float vrpos_hmdPosX = 0;
extern "C" float vrpos_hmdPosY = 0;
extern "C" float vrpos_hmdPosZ = 0;


extern vr::IVRSystem *gIVRSystem;

vr::TrackedDevicePose_t trackedDevicePoseHMD;

extern "C" void updateHMDposition(void)
{
	gIVRSystem->GetDeviceToAbsoluteTrackingPose(
		vr::TrackingUniverseStanding, 0, &trackedDevicePoseHMD, 1);

	vr::HmdMatrix34_t matrix = trackedDevicePoseHMD.mDeviceToAbsoluteTracking;

	vr::HmdVector3_t vector;
	vrpos_hmdPosX = vector.v[0] = matrix.m[0][3];
	vrpos_hmdPosY = vector.v[1] = matrix.m[1][3];
	vrpos_hmdPosZ = vector.v[2] = matrix.m[2][3];

	vr::HmdQuaternion_t q;
	vrpos_hmdRotW = q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
	vrpos_hmdPitch = q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
	vrpos_hmdYaw = q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
	vrpos_hmdRoll = q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);


	double heading = atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * pow(q.y,2) - 2 * pow(q.z,2)); // heading
	double roll = asin(2 * q.x * q.y + 2 * q.z * q.w); // originally attitude
	double pitch = atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * pow(q.x,2) - 2 * pow(q.z,2)); // originally bank


	vrpos_hmdPitch = pitch;
	vrpos_hmdYaw = heading;
	vrpos_hmdRoll = roll;


	// Logging for testing
	//std::cout << "POS X: " << vrpos_hmdPosX << "  ";
	//std::cout << "POS Y: " << vrpos_hmdPosY << "  ";
	//std::cout << "POS Z: " << vrpos_hmdPosZ << "\n\n";

	std::cout << "heading (Y): " << heading << "\n";
	std::cout << "roll: " << roll << "\n";
	std::cout << "pitch: " << pitch << "\n\n\n";
}
