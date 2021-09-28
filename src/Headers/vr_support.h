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


		// * TRACKING * //

#ifdef __cplusplus
extern "C" {
#endif
	extern double vrpos_hmdRotW; // Potentially useless
	extern double vrpos_hmdPitch;
	extern double vrpos_hmdYaw;
	extern double vrpos_hmdRoll;
	extern double vrpos_hmdPitchDelta;
	extern double vrpos_hmdYawDelta;
	extern double vrpos_hmdRollDelta;

	extern float vrpos_hmdPosX;
	extern float vrpos_hmdPosY;
	extern float vrpos_hmdPosZ;
	extern float vrpos_hmdPosXDelta;
	extern float vrpos_hmdPosYDelta;
	extern float vrpos_hmdPosZDelta;
#ifdef __cplusplus
}
#endif