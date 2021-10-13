#pragma once

		// * INPUT * //

// This is used for calling Get*ActionData, to tell SteamVR Input which control triggered
enum playerActions
{
	vrMoveXY = 0,
	vrCameraXY,
	vrJump,
	vrShoot,
	vrFistLeft,
	vrFistRight,
	vrPunchOrPickUp,
	vrPreviousWeapon,
	vrNextWeapon,
	vrEscapeMenu,
	vrLeftVibrate,
	vrRightVibrate,
	vrBothVibrate
};

#define VRminimumTriggerDefault 0.7f // How far the trigger has to be pulled for most trigger actions
#define VRminimumThumbstickDefault 0.4f // How far you must move thumbstick for most menu actions

// Multiply tracked VR device pos by this to get the equivalent game distance
// This number is APPROXIMATE and should be tweaked as playtesting happens
#define VRroomDistanceToGameDistanceScale 100

typedef struct
{
	float x;
	float y;
} vrJoyPos;

typedef struct
{
	double pitch;
	double yaw;
	double roll;
} vrEuler;

typedef struct
{
	float x;
	float y;
	float z;
} vrPosition;


		// * TRACKING * //

typedef struct
{
	int deviceID;

		/* ROTATION (pitch, yaw, roll) */
	vrEuler rot; // Current actual rotation
	vrEuler rotDelta; // Rotation delta (dif since last frame/last check)

		// Rotation special
	double HMDYawCorrected; // Only useful for HMD, use to correct yaw from thumbstick rotation


		/* POSITION (x, y, z) */
	vrPosition pos; // Current actual position
	vrPosition posDelta; // Position delta (dif since last frame/last check)
}TrackedVrDeviceInfo;

#ifdef __cplusplus
extern "C" {
#endif
	extern TrackedVrDeviceInfo vrInfoHMD;
	extern TrackedVrDeviceInfo vrInfoLeftHand;
	extern TrackedVrDeviceInfo vrInfoRightHand;
#ifdef __cplusplus
}
#endif