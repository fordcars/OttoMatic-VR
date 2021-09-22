#include "openvr.h"
#include "vr_support.h"
#include <iostream>
#include <cstring>
#include <SDL_filesystem.h>


struct VRActionHandlers
{
	vr::VRActionHandle_t GoForward;
	vr::VRActionHandle_t GoBackward;
	vr::VRActionHandle_t GoLeft;
	vr::VRActionHandle_t GoRight;
	vr::VRActionHandle_t MoveXY;
	vr::VRActionHandle_t CameraXY;
	vr::VRActionHandle_t Jump;
	vr::VRActionHandle_t Shoot;
	vr::VRActionHandle_t PunchOrPickUp;
	vr::VRActionHandle_t PreviousWeapon;
	vr::VRActionHandle_t NextWeapon;
	vr::VRActionHandle_t EscapeMenu;
};

static vr::VRInputValueHandle_t notRestrictedToHand = vr::k_ulInvalidInputValueHandle;

static VRActionHandlers vrActions{}; // init
static vr::VRActionSetHandle_t ottoVRactions;

static vr::VRActiveActionSet_t activeActionSet;

static vr::InputAnalogActionData_t goForwardAction{};
static vr::InputAnalogActionData_t goBackwardAction{};
static vr::InputAnalogActionData_t goLeftAction{};
static vr::InputAnalogActionData_t goRightAction{};
static vr::InputAnalogActionData_t moveXYAction{};
static vr::InputAnalogActionData_t cameraXYAction{};
static vr::InputDigitalActionData_t jumpAction{};
static vr::InputDigitalActionData_t shootAction{};
static vr::InputDigitalActionData_t punchOrPickupAction{};
static vr::InputDigitalActionData_t nextWeaponAction{};
static vr::InputDigitalActionData_t previousWeaponAction{};
static vr::InputDigitalActionData_t EscapeMenuAction{};


extern "C" void vrcpp_initSteamVRInput(void) {
	// Add path to action manifest for VR controls
	std::string actionPathCPP = std::string(SDL_GetBasePath()) + "Data\\steamvr\\actions.json";
	const char *actionPath = actionPathCPP.c_str();
	vr::VRInput()->SetActionManifestPath(actionPath);


	// Get handles for VR action sets and actions
	auto error = vr::VRInput()->GetActionSetHandle("/actions/otto", &ottoVRactions);
	if (error != vr::EVRInputError::VRInputError_None)
	{
		std::cerr << "GetActionSetHandle error.\n";
		printf("GetActionSetHandle error.\n");
	}


	vr::VRInput()->GetActionHandle("/actions/otto/in/GoForward", &vrActions.GoForward);
	vr::VRInput()->GetActionHandle("/actions/otto/in/GoBackward", &vrActions.GoBackward);
	vr::VRInput()->GetActionHandle("/actions/otto/in/GoLeft", &vrActions.GoLeft);
	vr::VRInput()->GetActionHandle("/actions/otto/in/GoRight", &vrActions.GoRight);
	vr::VRInput()->GetActionHandle("/actions/otto/in/MoveXY", &vrActions.MoveXY);
	vr::VRInput()->GetActionHandle("/actions/otto/in/CameraXY", &vrActions.CameraXY);
	error = vr::VRInput()->GetActionHandle("/actions/otto/in/Jump", &vrActions.Jump);
	if (error != vr::EVRInputError::VRInputError_None)
	{
		std::cerr << "GetActionHandle error.\n";
		printf("GetActionHandle error.\n");
	}
	vr::VRInput()->GetActionHandle("/actions/otto/in/Shoot", &vrActions.Shoot);
	vr::VRInput()->GetActionHandle("/actions/otto/in/PunchOrPickup", &vrActions.PunchOrPickUp);
	vr::VRInput()->GetActionHandle("/actions/otto/in/PreviousWeapon", &vrActions.PreviousWeapon);
	vr::VRInput()->GetActionHandle("/actions/otto/in/NextWeapon", &vrActions.NextWeapon);
	vr::VRInput()->GetActionHandle("/actions/otto/in/EscapeMenu", &vrActions.EscapeMenu);


	activeActionSet.ulActionSet = ottoVRactions;
	activeActionSet.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
	activeActionSet.nPriority = 0; // might be needed? Unsure
}

extern "C" void vrcpp_UpdateActionState(void) {
	auto error = vr::VRInput()->UpdateActionState(&activeActionSet, sizeof(activeActionSet), 1);
	if (error != vr::EVRInputError::VRInputError_None)
	{
		std::cerr << "Error UpdateActionState.\n";
	}
}



extern "C" vrJoyPos vrcpp_GetAnalogActionData(int actionToDo) {
	vr::VRActionHandle_t actionHandler;
	vr::InputAnalogActionData_t actionDataStruct;
	switch (actionToDo) {
	case playerActions::vrCameraXY:
		actionHandler = vrActions.CameraXY;
		actionDataStruct = cameraXYAction;
		break;
	case playerActions::vrMoveXY:
		actionHandler = vrActions.MoveXY;
		actionDataStruct = moveXYAction;
		break;
	default:
		printf("vrcpp_GetAnalogActionData called incorrectly");
		return { 0,0 };
	}

	// Get the state of the action
	vr::VRInput()->GetAnalogActionData(actionHandler, &actionDataStruct, sizeof(actionDataStruct), notRestrictedToHand);
	if (!actionDataStruct.bActive) {
		printf("GetAnalogActionData Problem, available to be bound: no\n"); // If this is printed, something wrong
	}

	// If any movement on any axis, return something other than 0
	if (actionDataStruct.x || actionDataStruct.y || actionDataStruct.z) {
		// For now all my actions are vector2 so only the XY is useful.
		// Must change this if we use vector3 actions eventually!!!
		return { actionDataStruct.x, actionDataStruct.y };
	}
	else {
		return { 0,0 };
	}
}



extern "C" bool vrcpp_GetDigitalActionData(int actionToDo) {
	vr::VRActionHandle_t actionHandler;
	vr::InputDigitalActionData_t actionDataStruct;
	switch (actionToDo) {
	case playerActions::vrJump:
		actionHandler = vrActions.Jump;
		actionDataStruct = jumpAction;
		break;
	case playerActions::vrShoot:
		actionHandler = vrActions.Shoot;
		actionDataStruct = shootAction;
		break;
	case playerActions::vrPunchOrPickUp:
		actionHandler = vrActions.PunchOrPickUp;
		actionDataStruct = punchOrPickupAction;
		break;
	case playerActions::vrPreviousWeapon:
		actionHandler = vrActions.PreviousWeapon;
		actionDataStruct = previousWeaponAction;
		break;
	case playerActions::vrNextWeapon:
		actionHandler = vrActions.NextWeapon;
		actionDataStruct = nextWeaponAction;
		break;
	case playerActions::vrEscapeMenu:
		actionHandler = vrActions.EscapeMenu;
		actionDataStruct = EscapeMenuAction;
		break;
	default:
		printf("vrcpp_GetDigitalActionData called incorrectly");
		return false;
	}

	// Get the state of the action
	vr::VRInput()->GetDigitalActionData(actionHandler, &actionDataStruct, sizeof(actionDataStruct), notRestrictedToHand);
	if (!actionDataStruct.bActive) {
		printf("vrcpp_GetDigitalActionData Problem, available to be bound: no\n"); // If this is printed, something wrong
	}
	if (actionDataStruct.bState && actionDataStruct.bChanged) {
		// printf("Changed\n"); // Use to test, prints if command detects
		return true;
	}
	else {
		return false;
	}
}
