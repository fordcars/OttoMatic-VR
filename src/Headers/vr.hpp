#pragma once
#include "openvr.h"

struct VrActionSets
{
	vr::VRActionSetHandle_t movement;
	vr::VRActionSetHandle_t weapons;
};


struct VrActions
{
	vr::VRActionHandle_t GoForward;
	vr::VRActionHandle_t GoBackward;
	vr::VRActionHandle_t GoLeft;
	vr::VRActionHandle_t GoRight;
	vr::VRActionHandle_t Jump;
	vr::VRActionHandle_t Shoot;
	vr::VRActionHandle_t PunchOrPickUp;
	vr::VRActionHandle_t PreviousWeapon;
	vr::VRActionHandle_t NextWeapon;
};
