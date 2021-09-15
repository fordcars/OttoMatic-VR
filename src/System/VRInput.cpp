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


VRActions vrActions{}; // init
vr::VRActionSetHandle_t ottoVRactions;



vr::VRActiveActionSet_t activeActionSet;
vr::InputDigitalActionData_t jumpAction{};
vr::InputDigitalActionData_t shootAction{};

extern "C" void initSteamVRInputCpp(void) {
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


extern "C" void updateVRActionSetStateCpp(void) {
	auto error = vr::VRInput()->UpdateActionState(&activeActionSet, sizeof(activeActionSet), 1);
	if (error != vr::EVRInputError::VRInputError_None)
	{
		std::cerr << "Error UpdateActionState.\n";
	}
}

extern "C" bool getVRDigitalActionDataCpp(int actionToDo) {
	if (actionToDo == playerActions::paJump) {
		auto e = vr::VRInput()->GetDigitalActionData(vrActions.Jump, &jumpAction, sizeof(jumpAction), vr::k_ulInvalidInputValueHandle);
		if (e != vr::EVRInputError::VRInputError_None)
		{
			// Print the rror code.
			std::cerr << e << '\n';
			std::cerr << "GetDigitalAction error.\n";
		}
		if (!jumpAction.bActive) {
			printf("Available to be bound: no\n"); // If this is printed, something wrong
		}
		//if (jumpAction.bState) {
		//	printf("IS PRESSED\n");
		//}
		//else if (!jumpAction.bChanged) {
		//	printf("State CHANGED\n");
		//}
		if (jumpAction.bState && jumpAction.bChanged) {
			//printf("Now PRESSED\n");
			return true;
		}
		//if (!jumpAction.bState && jumpAction.bChanged) {
		//	printf("Now RELEASED\n");
		//}
		return false;
	}
	if (actionToDo == playerActions::paShoot) {
		vr::VRInput()->GetDigitalActionData(vrActions.Shoot, &shootAction, sizeof(shootAction), vr::k_ulInvalidInputValueHandle);
		if (!shootAction.bActive) {
			printf("Available to be bound: no\n"); // If this is printed, something wrong
		}
		if (shootAction.bState && shootAction.bChanged) {
			return true;
		}

		return false;

	}
}