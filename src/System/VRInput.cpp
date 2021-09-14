#include "vr.hpp"




vr::VRActiveActionSet_t activeActionSet;


extern "C" void updateVRActionSetStateCpp(void) {
	vr::VRInput()->UpdateActionState(&activeActionSet, sizeof(activeActionSet), 1);
}
