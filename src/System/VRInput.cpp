#include "openvr.h"
#include "vr_support.h"
#include <iostream>
#include <cstring>
#include <SDL_filesystem.h>


struct VRActions
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

static vr::VRInputValueHandle_t notRestrictedToHand = vr::k_ulInvalidInputValueHandle;

static VRActions vrActions{}; // init
static vr::VRActionSetHandle_t ottoVRactions;



static vr::VRActiveActionSet_t activeActionSet;
static vr::InputDigitalActionData_t jumpAction{};
static vr::InputDigitalActionData_t shootAction{};
static vr::InputDigitalActionData_t punchOrPickupAction{};
static vr::InputDigitalActionData_t nextWeaponAction{};
static vr::InputDigitalActionData_t previousWeaponAction{};


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
	default:
		printf("vrcpp_GetDigitalActionData called incorrectly");
		return false;
	}

	// Get the state of the action
	vr::VRInput()->GetDigitalActionData(actionHandler, &actionDataStruct, sizeof(actionDataStruct), notRestrictedToHand);
	if (!actionDataStruct.bActive) {
		printf("Problem, available to be bound: no\n"); // If this is printed, something wrong
	}
	if (actionDataStruct.bState && actionDataStruct.bChanged) {
		return true;
	}
	else {
		return false;
	}


	//if (actionToDo == playerActions::vrJump) {
	//	auto e = vr::VRInput()->GetDigitalActionData(vrActions.Jump, &jumpAction, sizeof(jumpAction), notRestrictedToHand);
	//	if (e != vr::EVRInputError::VRInputError_None)
	//	{
	//		// Print the rror code.
	//		std::cerr << e << '\n';
	//		std::cerr << "GetDigitalAction error.\n";
	//	}
	//	if (!jumpAction.bActive) {
	//		printf("Available to be bound: no\n"); // If this is printed, something wrong
	//	}
	//	if (jumpAction.bState && jumpAction.bChanged) {
	//		//printf("Now PRESSED\n");
	//		return true;
	//	}
	//	//if (!jumpAction.bState && jumpAction.bChanged) {
	//	//	printf("Now RELEASED\n");
	//	//}
	//	return false;
	//}
	//if (actionToDo == playerActions::vrShoot) {
	//	vr::VRInput()->GetDigitalActionData(vrActions.Shoot, &shootAction, sizeof(shootAction), notRestrictedToHand);
	//	if (shootAction.bState && shootAction.bChanged) {
	//		return true;
	//	}
	//	else {
	//		return false;
	//	}
	//}
	//if (actionToDo == playerActions::vrPreviousWeapon) {
	//	vr::VRInput()->GetDigitalActionData(vrActions.PreviousWeapon, &previousWeaponAction, sizeof(previousWeaponAction), notRestrictedToHand);
	//	if (previousWeaponAction.bState && previousWeaponAction.bChanged) {
	//		return true;
	//	}
	//	else {
	//		return false;
	//	}
	//}
	//if (actionToDo == playerActions::vrNextWeapon) {
	//	vr::VRInput()->GetDigitalActionData(vrActions.NextWeapon, &nextWeaponAction, sizeof(nextWeaponAction), notRestrictedToHand);
	//	if (nextWeaponAction.bState && nextWeaponAction.bChanged) {
	//		return true;
	//	}
	//	else {
	//		return false;
	//	}
	//}
}