#pragma once

// This is used for calling Get*ActionData, to tell SteamVR Input which control triggered
enum playerActions
{
	vrGoForward = 0,
	vrGoBackward,
	vrGoLeft,
	vrGoRight,
	vrJump,
	vrShoot,
	vrPunchOrPickUp,
	vrPreviousWeapon,
	vrNextWeapon
};
