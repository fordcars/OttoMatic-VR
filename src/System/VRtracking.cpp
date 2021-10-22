#include "openvr.h"
#include <iostream>
#include <cstring>
#include "Pomme.h"
#include "PommeInit.h"
#include "PommeFiles.h"
#include "PommeGraphics.h"
#include "game.h"

extern "C" {
#include "ogl_support.h"
#include "SDL_opengl.h"
}


extern vr::IVRSystem *gIVRSystem;

vr::TrackedDevicePose_t trackedDevicePoseHMD;
vr::TrackedDevicePose_t trackedDevicePoseLeftHand;
vr::TrackedDevicePose_t trackedDevicePoseRightHand;
vr::TrackedDevicePose_t trackedDevices[vr::k_unMaxTrackedDeviceCount] = {};



TrackedVrDeviceInfo vrInfoHMD;
TrackedVrDeviceInfo vrInfoLeftHand;
TrackedVrDeviceInfo vrInfoRightHand;


void parseTrackingData(TrackedVrDeviceInfo *deviceToParse) {
	vr::HmdMatrix34_t trackedDeviceMatrix = trackedDevices[deviceToParse->deviceID].mDeviceToAbsoluteTracking;

	double devicePosXSinceLastUpdate = deviceToParse->pos.x;
	double devicePosYSinceLastUpdate = deviceToParse->pos.y;
	double devicePosZSinceLastUpdate = deviceToParse->pos.z;

	vr::HmdVector3_t vector;
	deviceToParse->pos.x = vector.v[0] = trackedDeviceMatrix.m[0][3];
	deviceToParse->pos.y = vector.v[1] = trackedDeviceMatrix.m[1][3];
	deviceToParse->pos.z = vector.v[2] = trackedDeviceMatrix.m[2][3];

	deviceToParse->posDelta.x = -(devicePosXSinceLastUpdate - deviceToParse->pos.x);
	deviceToParse->posDelta.y = devicePosYSinceLastUpdate - deviceToParse->pos.y;
	deviceToParse->posDelta.z = -(devicePosZSinceLastUpdate - deviceToParse->pos.z);


	vr::HmdQuaternion_t q;
	deviceToParse->quat.w = q.w = sqrt(fmax(0, 1 + trackedDeviceMatrix.m[0][0] + trackedDeviceMatrix.m[1][1] + trackedDeviceMatrix.m[2][2])) / 2;
	deviceToParse->quat.x = q.x = sqrt(fmax(0, 1 + trackedDeviceMatrix.m[0][0] - trackedDeviceMatrix.m[1][1] - trackedDeviceMatrix.m[2][2])) / 2;
	deviceToParse->quat.y = q.y = sqrt(fmax(0, 1 - trackedDeviceMatrix.m[0][0] + trackedDeviceMatrix.m[1][1] - trackedDeviceMatrix.m[2][2])) / 2;
	deviceToParse->quat.z = q.z = sqrt(fmax(0, 1 - trackedDeviceMatrix.m[0][0] - trackedDeviceMatrix.m[1][1] + trackedDeviceMatrix.m[2][2])) / 2;
	deviceToParse->quat.x = q.x = copysign(q.x, trackedDeviceMatrix.m[2][1] - trackedDeviceMatrix.m[1][2]);
	deviceToParse->quat.y = q.y = copysign(q.y, trackedDeviceMatrix.m[0][2] - trackedDeviceMatrix.m[2][0]);
	deviceToParse->quat.z = q.z = copysign(q.z, trackedDeviceMatrix.m[1][0] - trackedDeviceMatrix.m[0][1]);


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

	deviceToParse->rawVRmatrix.m[0][0] = trackedDeviceMatrix.m[0][0];
	deviceToParse->rawVRmatrix.m[0][1] = trackedDeviceMatrix.m[0][1];
	deviceToParse->rawVRmatrix.m[0][2] = trackedDeviceMatrix.m[0][2];
	deviceToParse->rawVRmatrix.m[0][3] = trackedDeviceMatrix.m[0][3];
	deviceToParse->rawVRmatrix.m[1][0] = trackedDeviceMatrix.m[1][0];
	deviceToParse->rawVRmatrix.m[1][1] = trackedDeviceMatrix.m[1][1];
	deviceToParse->rawVRmatrix.m[1][2] = trackedDeviceMatrix.m[1][2];
	deviceToParse->rawVRmatrix.m[1][3] = trackedDeviceMatrix.m[1][3];
	deviceToParse->rawVRmatrix.m[2][0] = trackedDeviceMatrix.m[2][0];
	deviceToParse->rawVRmatrix.m[2][1] = trackedDeviceMatrix.m[2][1];
	deviceToParse->rawVRmatrix.m[2][2] = trackedDeviceMatrix.m[2][2];
	deviceToParse->rawVRmatrix.m[2][3] = trackedDeviceMatrix.m[2][3];

	deviceToParse->transformationMatrix.value[M00] = deviceToParse->rawVRmatrix.m[0][0];
	deviceToParse->transformationMatrix.value[M01] = deviceToParse->rawVRmatrix.m[0][1];
	deviceToParse->transformationMatrix.value[M02] = deviceToParse->rawVRmatrix.m[0][2];
	deviceToParse->transformationMatrix.value[M03] = deviceToParse->rawVRmatrix.m[0][3]; // Unused for controllers -> Translation X
	deviceToParse->transformationMatrix.value[M10] = deviceToParse->rawVRmatrix.m[1][0];
	deviceToParse->transformationMatrix.value[M11] = deviceToParse->rawVRmatrix.m[1][1];
	deviceToParse->transformationMatrix.value[M12] = deviceToParse->rawVRmatrix.m[1][2];
	deviceToParse->transformationMatrix.value[M13] = deviceToParse->rawVRmatrix.m[1][3]; // Unused for controllers -> Translation Y
	deviceToParse->transformationMatrix.value[M20] = deviceToParse->rawVRmatrix.m[2][0];
	deviceToParse->transformationMatrix.value[M21] = deviceToParse->rawVRmatrix.m[2][1];
	deviceToParse->transformationMatrix.value[M22] = deviceToParse->rawVRmatrix.m[2][2];
	deviceToParse->transformationMatrix.value[M23] = deviceToParse->rawVRmatrix.m[2][3]; // Unused for controllers -> Translation Z
	deviceToParse->transformationMatrix.value[M30] = 0;
	deviceToParse->transformationMatrix.value[M31] = 0;
	deviceToParse->transformationMatrix.value[M32] = 0;
	deviceToParse->transformationMatrix.value[M33] = 1;


	// Update the HMD specific members
	vrInfoHMD.HMDgameYawIgnoringHMD = vrInfoHMD.HMDYawCorrected - vrInfoHMD.rot.yaw;

	vrInfoHMD.HMDgameYawCorrectionMatrix = { 0 };
	vrInfoHMD.HMDgameYawCorrectionMatrix.value[M00] = cos(-vrInfoHMD.HMDgameYawIgnoringHMD);
	vrInfoHMD.HMDgameYawCorrectionMatrix.value[M02] = -sin(-vrInfoHMD.HMDgameYawIgnoringHMD);
	vrInfoHMD.HMDgameYawCorrectionMatrix.value[M20] = sin(-vrInfoHMD.HMDgameYawIgnoringHMD);
	vrInfoHMD.HMDgameYawCorrectionMatrix.value[M22] = cos(-vrInfoHMD.HMDgameYawIgnoringHMD);
	vrInfoHMD.HMDgameYawCorrectionMatrix.value[M11] = 1;
	vrInfoHMD.HMDgameYawCorrectionMatrix.value[M33] = 1;

	// Calculate the HMD's corrected matrix
	OGLMatrix4x4_Multiply(&vrInfoHMD.transformationMatrix, &vrInfoHMD.HMDgameYawCorrectionMatrix, &vrInfoHMD.transformationMatrixCorrected);


	// Calculate the HMD's corrected matrix
	OGLMatrix4x4 rotOnly = deviceToParse->transformationMatrixCorrected;
	OGLMatrix4x4 transOnly = { 0 };
	rotOnly.value[M03] = 0;
	rotOnly.value[M13] = 0;
	rotOnly.value[M23] = 0;

	transOnly.value[M03] = deviceToParse->transformationMatrix.value[M03];
	transOnly.value[M13] = deviceToParse->transformationMatrix.value[M13];
	transOnly.value[M23] = deviceToParse->transformationMatrix.value[M23];
	transOnly.value[M33] = 1;
	transOnly.value[M22] = 1;
	transOnly.value[M11] = 1;
	transOnly.value[M00] = 1;

	deviceToParse->rotationMatrixCorrected = rotOnly;
	deviceToParse->translationMatrix = transOnly;
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
	{
	//vrInfoHMD.pos.x = 0;
	//vrInfoHMD.pos.y = 1.5;
	//vrInfoHMD.pos.z = 1;

	//vrInfoLeftHand.pos.x = 0;
	//vrInfoLeftHand.pos.y = 2.5;
	//vrInfoLeftHand.pos.z = 0;

	//vrInfoLeftHand.rot.pitch = PI / 2;
	//vrInfoLeftHand.rot.yaw += 0.01;
	//vrInfoLeftHand.rot.roll = 0;
	}

	// Spinning in front of you on the roll axis while pointing forward
	{
		//vrInfoHMD.pos.x = 0;
		//vrInfoHMD.pos.y = 1.5;
		//vrInfoHMD.pos.z = 1;

		//vrInfoLeftHand.pos.x = 0;
		//vrInfoLeftHand.pos.y = 2.5;
		//vrInfoLeftHand.pos.z = 0;

		//vrInfoLeftHand.rot.pitch += 0.01;
		//vrInfoLeftHand.rot.yaw = 0;
		//vrInfoLeftHand.rot.roll = 0.01;
	}
	// FOR TESTING ONLY, DISABLE WHEN USING REAL CONTROLLERS!!!!!!!!!!!!!!!!!!!!!



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
	double HMDposGameAxesXSinceLastUpdate = vrInfoHMD.posGameAxes.x;
	double HMDposGameAxesZSinceLastUpdate = vrInfoHMD.posGameAxes.z;
	double LeftHandposGameAxesXSinceLastUpdate = vrInfoLeftHand.posGameAxes.x;
	double LeftHandposGameAxesZSinceLastUpdate = vrInfoLeftHand.posGameAxes.z;
	double RightHandposGameAxesXSinceLastUpdate = vrInfoRightHand.posGameAxes.x;
	double RightHandposGameAxesZSinceLastUpdate = vrInfoRightHand.posGameAxes.z;
	
	
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

	vrInfoHMD.posDeltaGameAxes.x = -(HMDposGameAxesXSinceLastUpdate - vrInfoHMD.posGameAxes.x);
	vrInfoHMD.posDeltaGameAxes.z = -(HMDposGameAxesZSinceLastUpdate - vrInfoHMD.posGameAxes.z);
	vrInfoLeftHand.posDeltaGameAxes.x = -(LeftHandposGameAxesXSinceLastUpdate - vrInfoLeftHand.posGameAxes.x);
	vrInfoLeftHand.posDeltaGameAxes.z = -(LeftHandposGameAxesZSinceLastUpdate - vrInfoLeftHand.posGameAxes.z);
	vrInfoRightHand.posDeltaGameAxes.x = -(RightHandposGameAxesXSinceLastUpdate - vrInfoRightHand.posGameAxes.x);
	vrInfoRightHand.posDeltaGameAxes.z = -(RightHandposGameAxesZSinceLastUpdate - vrInfoRightHand.posGameAxes.z);
}


extern "C" void getXYZforCamera() {
	TrackedVrDeviceInfo deviceToParse = vrInfoHMD;
	vrMatrix34 trackedDeviceMatrix = vrInfoHMD.rawVRmatrix;
	vr::HmdQuaternion_t q;
	deviceToParse.quat.w = q.w = sqrt(fmax(0, 1 + trackedDeviceMatrix.m[0][0] + trackedDeviceMatrix.m[1][1] + trackedDeviceMatrix.m[2][2])) / 2;
	deviceToParse.quat.x = q.x = sqrt(fmax(0, 1 + trackedDeviceMatrix.m[0][0] - trackedDeviceMatrix.m[1][1] - trackedDeviceMatrix.m[2][2])) / 2;
	deviceToParse.quat.y = q.y = sqrt(fmax(0, 1 - trackedDeviceMatrix.m[0][0] + trackedDeviceMatrix.m[1][1] - trackedDeviceMatrix.m[2][2])) / 2;
	deviceToParse.quat.z = q.z = sqrt(fmax(0, 1 - trackedDeviceMatrix.m[0][0] - trackedDeviceMatrix.m[1][1] + trackedDeviceMatrix.m[2][2])) / 2;
	deviceToParse.quat.x = q.x = copysign(q.x, trackedDeviceMatrix.m[2][1] - trackedDeviceMatrix.m[1][2]);
	deviceToParse.quat.y = q.y = copysign(q.y, trackedDeviceMatrix.m[0][2] - trackedDeviceMatrix.m[2][0]);
	deviceToParse.quat.z = q.z = copysign(q.z, trackedDeviceMatrix.m[1][0] - trackedDeviceMatrix.m[0][1]);
}