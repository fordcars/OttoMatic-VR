#pragma once

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

typedef struct
{
	float x;
	float y;
} vrJoyPos;
