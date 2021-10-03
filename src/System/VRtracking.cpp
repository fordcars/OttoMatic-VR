#include "openvr.h"
#include "vr_support.h"
#include <iostream>
#include <cstring>

// Rotation vars
extern "C" double vrpos_hmdRotW = 0;
extern "C" double vrpos_hmdPitch = 0;
extern "C" double vrpos_hmdYaw = 0;
extern "C" double vrpos_hmdRoll = 0;
// Rotation delta vars
extern "C" double vrpos_hmdPitchDelta = 0;
extern "C" double vrpos_hmdYawDelta = 0;
extern "C" double vrpos_hmdRollDelta = 0;
// Rotation special
extern "C" double vrpos_hmdYawCorrected = 0;


// Position vars
extern "C" float vrpos_hmdPosX = 0;
extern "C" float vrpos_hmdPosY = 0;
extern "C" float vrpos_hmdPosZ = 0;
// Position delta vars
extern "C" float vrpos_hmdPosXDelta = 0;
extern "C" float vrpos_hmdPosYDelta = 0;
extern "C" float vrpos_hmdPosZDelta = 0;


extern vr::IVRSystem *gIVRSystem;

vr::TrackedDevicePose_t trackedDevicePoseHMD;

extern "C" void updateHMDposition(void)
{
	gIVRSystem->GetDeviceToAbsoluteTrackingPose(
		vr::TrackingUniverseStanding, 0, &trackedDevicePoseHMD, 1);

	vr::HmdMatrix34_t matrix = trackedDevicePoseHMD.mDeviceToAbsoluteTracking;

	double vrpos_hmdPosXSinceLastUpdate = vrpos_hmdPosX;
	double vrpos_hmdPosYSinceLastUpdate = vrpos_hmdPosY;
	double vrpos_hmdPosZSinceLastUpdate = vrpos_hmdPosZ;

	vr::HmdVector3_t vector;
	vrpos_hmdPosX = vector.v[0] = matrix.m[0][3];
	vrpos_hmdPosY = vector.v[1] = matrix.m[1][3];
	vrpos_hmdPosZ = vector.v[2] = matrix.m[2][3];

	vrpos_hmdPosXDelta = -(vrpos_hmdPosXSinceLastUpdate - vrpos_hmdPosX);
	vrpos_hmdPosYDelta = vrpos_hmdPosYSinceLastUpdate - vrpos_hmdPosY;
	vrpos_hmdPosZDelta = -(vrpos_hmdPosZSinceLastUpdate - vrpos_hmdPosZ);


	vr::HmdQuaternion_t q;
	vrpos_hmdRotW = q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
	q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
	q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);


	// Get the Euler angles from the last HMD update and remember them
	double vrpos_hmdPitchSinceLastUpdate = vrpos_hmdPitch;
	double vrpos_hmdYawSinceLastUpdate = vrpos_hmdYaw;
	double vrpos_hmdRollSinceLastUpdate = vrpos_hmdRoll;

	// Update the vrpos vars with headset rotation
	vrpos_hmdPitch = atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * pow(q.x, 2) - 2 * pow(q.z, 2)); // originally bank
	vrpos_hmdYaw = atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * pow(q.y, 2) - 2 * pow(q.z, 2));
	vrpos_hmdRoll = asin(2 * q.x * q.y + 2 * q.z * q.w); // originally attitude

	// Calculate the difference between current and last HMD rotation to get delta
	vrpos_hmdPitchDelta = vrpos_hmdPitchSinceLastUpdate - vrpos_hmdPitch;
	vrpos_hmdYawDelta = vrpos_hmdYawSinceLastUpdate - vrpos_hmdYaw;
	vrpos_hmdRollDelta = vrpos_hmdRollSinceLastUpdate - vrpos_hmdRoll;


	// Logging for testing
	//std::cout << "POS X: " << vrpos_hmdPosX << "  ";
	//std::cout << "POS Y: " << vrpos_hmdPosY << "  ";
	//std::cout << "POS Z: " << vrpos_hmdPosZ << "\n\n";

	//std::cout << "heading (yaw): " << heading << "\n";
	//std::cout << "roll: " << roll << "\n";
	//std::cout << "pitch: " << pitch << "\n\n\n";

	//printf("heading (yaw): %f\n", vrpos_hmdYaw);
	//printf("heading (yaw) SinceLastUpdate: %f\n", vrpos_hmdYawDelta);
	//printf("pitch: %f\n", vrpos_hmdPitch);
	//printf("roll: %f\n\n", vrpos_hmdRoll);

	//printf("vrpos_hmdPosX: %f\n", vrpos_hmdPosX);
	//printf("vrpos_hmdPosY: %f\n", vrpos_hmdPosY);
	//printf("vrpos_hmdPosZ: %f\n", vrpos_hmdPosZ);
	//printf("vrpos_hmdPosXDelta: %f\n", vrpos_hmdPosXDelta);
	//printf("vrpos_hmdPosYDelta: %f\n", vrpos_hmdPosYDelta);
	//printf("vrpos_hmdPosZDelta: %f\n", vrpos_hmdPosZDelta);
}
