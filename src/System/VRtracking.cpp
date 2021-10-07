#include "openvr.h"
#include "vr_support.h"
#include <iostream>
#include <cstring>



extern vr::IVRSystem *gIVRSystem;

vr::TrackedDevicePose_t trackedDevicePoseHMD;

TrackedVrDeviceInfo vrInfoHMD;

extern "C" void updateHMDposition(void)
{
	gIVRSystem->GetDeviceToAbsoluteTrackingPose(
		vr::TrackingUniverseStanding, 0, &trackedDevicePoseHMD, 1);

	vr::HmdMatrix34_t matrix = trackedDevicePoseHMD.mDeviceToAbsoluteTracking;

	double vrpos_hmdPosXSinceLastUpdate = vrInfoHMD.pos.x;
	double vrpos_hmdPosYSinceLastUpdate = vrInfoHMD.pos.y;
	double vrpos_hmdPosZSinceLastUpdate = vrInfoHMD.pos.z;

	vr::HmdVector3_t vector;
	vrInfoHMD.pos.x = vector.v[0] = matrix.m[0][3];
	vrInfoHMD.pos.y = vector.v[1] = matrix.m[1][3];
	vrInfoHMD.pos.z = vector.v[2] = matrix.m[2][3];

	vrInfoHMD.posDelta.x = -(vrpos_hmdPosXSinceLastUpdate - vrInfoHMD.pos.x);
	vrInfoHMD.posDelta.y = vrpos_hmdPosYSinceLastUpdate - vrInfoHMD.pos.y;
	vrInfoHMD.posDelta.z = -(vrpos_hmdPosZSinceLastUpdate - vrInfoHMD.pos.z);


	vr::HmdQuaternion_t q;
	q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
	q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
	q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);


	// Get the Euler angles from the last HMD update and remember them
	double vrpos_hmdPitchSinceLastUpdate = vrInfoHMD.rot.pitch;
	double vrpos_hmdYawSinceLastUpdate = vrInfoHMD.rot.yaw;
	double vrpos_hmdRollSinceLastUpdate = vrInfoHMD.rot.roll;

	// Update the vrpos vars with headset rotation
	vrInfoHMD.rot.pitch = atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * pow(q.x, 2) - 2 * pow(q.z, 2)); // originally bank
	vrInfoHMD.rot.yaw = atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * pow(q.y, 2) - 2 * pow(q.z, 2));
	vrInfoHMD.rot.roll = asin(2 * q.x * q.y + 2 * q.z * q.w); // originally attitude

	// Calculate the difference between current and last HMD rotation to get delta
	vrInfoHMD.rotDelta.pitch = vrpos_hmdPitchSinceLastUpdate - vrInfoHMD.rot.pitch;
	vrInfoHMD.rotDelta.yaw = vrpos_hmdYawSinceLastUpdate - vrInfoHMD.rot.yaw;
	vrInfoHMD.rotDelta.roll = vrpos_hmdRollSinceLastUpdate - vrInfoHMD.rot.roll;


	// Logging for testing
	//std::cout << "POS X: " << vrInfoHMD.pos.x << "  ";
	//std::cout << "POS Y: " << vrInfoHMD.pos.y << "  ";
	//std::cout << "POS Z: " << vrInfoHMD.pos.z << "\n\n";

	//std::cout << "heading (yaw): " << vrInfoHMD.rot.yaw << "\n";
	//std::cout << "roll: " << vrInfoHMD.rot.roll << "\n";
	//std::cout << "pitch: " << vrInfoHMD.rot.pitch << "\n\n\n";

	//printf("heading (yaw): %f\n", vrInfoHMD.rot.yaw);
	//printf("heading (yaw) SinceLastUpdate: %f\n", vrInfoHMD.rotDelta.yaw);
	//printf("pitch: %f\n", vrInfoHMD.rot.pitch);
	//printf("roll: %f\n\n", vrInfoHMD.rot.roll);

	//printf("pos.x: %f\n", vrInfoHMD.pos.x);
	//printf("pos.y: %f\n", vrInfoHMD.pos.y);
	//printf("pos.z: %f\n", vrInfoHMD.pos.z);
	//printf("posDelta.x: %f\n", vrInfoHMD.posDelta.x);
	//printf("posDelta.y: %f\n", vrInfoHMD.posDelta.y);
	//printf("posDelta.z: %f\n", vrInfoHMD.posDelta.z);
}
