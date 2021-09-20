#pragma once

// This is used for calling Get*ActionData, to tell SteamVR Input which control triggered
enum playerActions
{
	vrGoForward = 0,
	vrGoBackward,
	vrGoLeft,
	vrGoRight,
	vrMoveXY,
	vrCameraXY,
	vrJump,
	vrShoot,
	vrPunchOrPickUp,
	vrPreviousWeapon,
	vrNextWeapon
};

typedef struct
{
	float x;
	float y;
} vrJoyPos;
