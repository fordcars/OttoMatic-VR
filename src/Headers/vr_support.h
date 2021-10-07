#pragma once

		// * INPUT * //

// This is used for calling Get*ActionData, to tell SteamVR Input which control triggered
enum playerActions
{
	vrMoveXY = 0,
	vrCameraXY,
	vrJump,
	vrShoot,
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
} vrPostion;


		// * TRACKING * //

typedef struct
{
		/* ROTATION (pitch, yaw, roll) */
	vrEuler rot; // Current actual rotation
	vrEuler rotDelta; // Rotation delta (dif since last frame/last check)

	// Rotation special
	double HMDYawCorrected; // Only useful for HMD, use to correct yaw from thumbstick rotation


		/* POSITION (x, y, z) */
	vrPostion pos; // Current actual position
	vrPostion posDelta; // Position delta (dif since last frame/last check)

}TrackedVrDeviceInfo;

#ifdef __cplusplus
extern "C" {
#endif
	extern TrackedVrDeviceInfo vrInfoHMD;
#ifdef __cplusplus
}
#endif