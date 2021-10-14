#include "openvr.h"
#include "vr_support.h"
#include <iostream>
#include <cstring>

#define	PI					((float)3.1415926535898)

extern vr::IVRSystem *gIVRSystem;

vr::TrackedDevicePose_t trackedDevicePoseHMD;
vr::TrackedDevicePose_t trackedDevicePoseLeftHand;
vr::TrackedDevicePose_t trackedDevicePoseRightHand;
vr::TrackedDevicePose_t trackedDevices[vr::k_unMaxTrackedDeviceCount] = {};



TrackedVrDeviceInfo vrInfoHMD;
TrackedVrDeviceInfo vrInfoLeftHand;
TrackedVrDeviceInfo vrInfoRightHand;


void parseTrackingData(TrackedVrDeviceInfo *deviceToParse) {
	vr::HmdMatrix34_t matrix = trackedDevices[deviceToParse->deviceID].mDeviceToAbsoluteTracking;

	double devicePosXSinceLastUpdate = deviceToParse->pos.x;
	double devicePosYSinceLastUpdate = deviceToParse->pos.y;
	double devicePosZSinceLastUpdate = deviceToParse->pos.z;

	vr::HmdVector3_t vector;
	deviceToParse->pos.x = vector.v[0] = matrix.m[0][3];
	deviceToParse->pos.y = vector.v[1] = matrix.m[1][3];
	deviceToParse->pos.z = vector.v[2] = matrix.m[2][3];

	deviceToParse->posDelta.x = -(devicePosXSinceLastUpdate - deviceToParse->pos.x);
	deviceToParse->posDelta.y = devicePosYSinceLastUpdate - deviceToParse->pos.y;
	deviceToParse->posDelta.z = -(devicePosZSinceLastUpdate - deviceToParse->pos.z);


	vr::HmdQuaternion_t q;
	q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
	q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
	q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);


	// Get the Euler angles from the last HMD update and remember them
	double devicePitchSinceLastUpdate = deviceToParse->rot.pitch;
	double deviceYawSinceLastUpdate = deviceToParse->rot.yaw;
	double deviceRollSinceLastUpdate = deviceToParse->rot.roll;

	// Update the vrpos vars with headset rotation
	deviceToParse->rot.pitch = atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * pow(q.x, 2) - 2 * pow(q.z, 2)); // originally bank
	deviceToParse->rot.yaw = atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * pow(q.y, 2) - 2 * pow(q.z, 2));
	deviceToParse->rot.roll = asin(2 * q.x * q.y + 2 * q.z * q.w); // originally attitude

	// Calculate the difference between current and last HMD rotation to get delta
	deviceToParse->rotDelta.pitch = devicePitchSinceLastUpdate - deviceToParse->rot.pitch;
	deviceToParse->rotDelta.yaw = deviceYawSinceLastUpdate - deviceToParse->rot.yaw;
	deviceToParse->rotDelta.roll = deviceRollSinceLastUpdate - deviceToParse->rot.roll;
}



extern "C" void updateHMDposition(void)
{
	int numberOfTrackedDevices = 0;
	// Check for all VR devices
	for (int deviceID = 0; deviceID < vr::k_unMaxTrackedDeviceCount; deviceID++) {
		vr::ETrackedDeviceClass deviceClass = gIVRSystem->GetTrackedDeviceClass(deviceID);
		if (deviceClass != vr::TrackedDeviceClass_Invalid) {
			// Count how many tracked devices we have
			numberOfTrackedDevices++;
		}
		// We only care about HMD and controllers, so ignore trackers and references
		if (deviceClass == vr::TrackedDeviceClass_Controller) {
			// std::cout << "ID #" << deviceID << " is of type " << deviceClass << std::endl;
			vr::ETrackedControllerRole role =
				gIVRSystem->GetControllerRoleForTrackedDeviceIndex(deviceID);
			if (role == vr::TrackedControllerRole_Invalid) {
				// The controller is probably not visible to a base station.
			}
			else if (role == vr::TrackedControllerRole_LeftHand)
			{
				vrInfoLeftHand.deviceID = deviceID;
			}
			else if (role == vr::TrackedControllerRole_RightHand)
			{
				vrInfoRightHand.deviceID = deviceID;
			}
		}
		else if (deviceClass == vr::TrackedDeviceClass_HMD) {
			// std::cout << "ID #" << deviceID << " is of type " << deviceClass << std::endl;
			vrInfoHMD.deviceID = deviceID;
		}
	}


	// Actually GET the position data
	gIVRSystem->GetDeviceToAbsoluteTrackingPose(
		vr::TrackingUniverseStanding, 0, trackedDevices, numberOfTrackedDevices);


	// Parse it
	parseTrackingData(&vrInfoHMD);
	// Only parse controller data if controllers exist
	if (vrInfoLeftHand.deviceID)
		parseTrackingData(&vrInfoLeftHand);
	if (vrInfoRightHand.deviceID)
		parseTrackingData(&vrInfoRightHand);


	// FOR TESTING ONLY, DISABLE WHEN USING REAL CONTROLLERS!!!!!!!!!!!!!!!!!!!!!
	// Spinning in front of you on the yaw axis while pointing forward
	//{
	//vrInfoHMD.pos.x = 0;
	//vrInfoHMD.pos.y = 1.5;
	//vrInfoHMD.pos.z = 1;

	//vrInfoLeftHand.pos.x = 0;
	//vrInfoLeftHand.pos.y = 2.5;
	//vrInfoLeftHand.pos.z = 0;

	//vrInfoLeftHand.rot.pitch = PI / 2;
	//vrInfoLeftHand.rot.yaw += 0.01;
	//vrInfoLeftHand.rot.roll = 0;
	//}

	// Spinning in front of you on the roll axis while pointing forward
	//{
	//	vrInfoHMD.pos.x = 0;
	//	vrInfoHMD.pos.y = 1.5;
	//	vrInfoHMD.pos.z = 1;

	//	vrInfoLeftHand.pos.x = 0;
	//	vrInfoLeftHand.pos.y = 2.5;
	//	vrInfoLeftHand.pos.z = 0;

	//	vrInfoLeftHand.rot.pitch = 0.01;
	//	vrInfoLeftHand.rot.yaw = 0;
	//	vrInfoLeftHand.rot.roll += 0.01;
	//}
	// FOR TESTING ONLY, DISABLE WHEN USING REAL CONTROLLERS!!!!!!!!!!!!!!!!!!!!!

	// Update the HMD specific members
	vrInfoHMD.HMDgameYawIgnoringHMD = vrInfoHMD.HMDYawCorrected - vrInfoHMD.rot.yaw;





		/* Logging for testing */

	//printf("HMD heading (yaw): %f\n", vrInfoHMD.rot.yaw);
	//printf("HMD heading (yaw) SinceLastUpdate: %f\n", vrInfoHMD.rotDelta.yaw);
	//printf("HMD pitch: %f\n", vrInfoHMD.rot.pitch);
	//printf("HMD roll: %f\n\n", vrInfoHMD.rot.roll);

	//printf("HMD pos.x: %f\n", vrInfoHMD.pos.x);
	//printf("HMD pos.y: %f\n", vrInfoHMD.pos.y);
	//printf("HMD pos.z: %f\n", vrInfoHMD.pos.z);
	//printf("HMD posDelta.x: %f\n", vrInfoHMD.posDelta.x);
	//printf("HMD posDelta.y: %f\n", vrInfoHMD.posDelta.y);
	//printf("HMD posDelta.z: %f\n", vrInfoHMD.posDelta.z);

	//printf("LeftHand yaw: %f    RightHand yaw: %f\n", vrInfoLeftHand.rot.yaw, vrInfoRightHand.rot.yaw);
	//printf("LeftHand pitch: %f    RightHand pitch: %f\n", vrInfoLeftHand.rot.pitch, vrInfoRightHand.rot.pitch);
	//printf("LeftHand roll: %f    RightHand roll: %f\n\n", vrInfoLeftHand.rot.roll, vrInfoRightHand.rot.roll);

	//printf("LeftHand pos.x: %f    RightHand pos.x: %f\n", vrInfoLeftHand.pos.x, vrInfoRightHand.pos.x);
	//printf("LeftHand pos.y: %f    RightHand pos.y: %f\n", vrInfoLeftHand.pos.y, vrInfoRightHand.pos.y);
	//printf("LeftHand pos.z: %f    RightHand pos.z: %f\n\n", vrInfoLeftHand.pos.z, vrInfoRightHand.pos.z);

	//printf("LeftHand posDelta.x: %f    RightHand posDelta.x: %f\n", vrInfoLeftHand.posDelta.x, vrInfoRightHand.posDelta.x);
	//printf("LeftHand posDelta.y: %f    RightHand posDelta.y: %f\n", vrInfoLeftHand.posDelta.y, vrInfoRightHand.posDelta.y);
	//printf("LeftHand posDelta.z: %f    RightHand posDelta.z: %f\n\n\n", vrInfoLeftHand.posDelta.z, vrInfoRightHand.posDelta.z);

}

extern "C" void updateGameSpacePositions() {
	vrInfoHMD.posGameAxes.x =
		vrInfoHMD.pos.x * cos(vrInfoHMD.HMDgameYawIgnoringHMD) + vrInfoHMD.pos.z * sin(vrInfoHMD.HMDgameYawIgnoringHMD);
	vrInfoHMD.posGameAxes.z =
		vrInfoHMD.pos.z * cos(vrInfoHMD.HMDgameYawIgnoringHMD) - vrInfoHMD.pos.x * sin(vrInfoHMD.HMDgameYawIgnoringHMD);

	vrInfoLeftHand.posGameAxes.x =
		vrInfoLeftHand.pos.x * cos(vrInfoHMD.HMDgameYawIgnoringHMD) + vrInfoLeftHand.pos.z * sin(vrInfoHMD.HMDgameYawIgnoringHMD);
	vrInfoLeftHand.posGameAxes.z =
		vrInfoLeftHand.pos.z * cos(vrInfoHMD.HMDgameYawIgnoringHMD) - vrInfoLeftHand.pos.x * sin(vrInfoHMD.HMDgameYawIgnoringHMD);

	vrInfoRightHand.posGameAxes.x =
		vrInfoRightHand.pos.x * cos(vrInfoHMD.HMDgameYawIgnoringHMD) + vrInfoRightHand.pos.z * sin(vrInfoHMD.HMDgameYawIgnoringHMD);
	vrInfoRightHand.posGameAxes.z =
		vrInfoRightHand.pos.z * cos(vrInfoHMD.HMDgameYawIgnoringHMD) - vrInfoRightHand.pos.x * sin(vrInfoHMD.HMDgameYawIgnoringHMD);
}