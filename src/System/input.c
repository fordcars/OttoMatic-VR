// INPUT.C
// (c)2021 Iliyas Jorio
// This file is part of Otto Matic. https://github.com/jorio/ottomatic

#include "game.h"
#include "mousesmoothing.h"
#include "killmacmouseacceleration.h"


/***************/
/* CONSTANTS   */
/***************/

enum
{
	KEYSTATE_OFF		= 0b00,
	KEYSTATE_UP			= 0b01,

	KEYSTATE_PRESSED	= 0b10,
	KEYSTATE_HELD		= 0b11,

	KEYSTATE_ACTIVE_BIT	= 0b10,
};

#define kJoystickDeadZone				(33 * 32767 / 100)
#define kJoystickDeadZone_UI			(66 * 32767 / 100)
#define kJoystickDeadZoneFrac			(kJoystickDeadZone / 32767.0f)
#define kJoystickDeadZoneFracSquared	(kJoystickDeadZoneFrac * kJoystickDeadZoneFrac)

#define kDefaultSnapAngle				OGLMath_DegreesToRadians(10)

/***************/
/* EXTERNALS   */
/***************/

/**********************/
/*     PROTOTYPES     */
/**********************/

Boolean				gUserPrefersGamepad = false;

SDL_GameController	*gSDLController = NULL;
SDL_JoystickID		gSDLJoystickInstanceID = -1;		// ID of the joystick bound to gSDLController

Byte				gRawKeyboardState[SDL_NUM_SCANCODES];
char				gTextInput[SDL_TEXTINPUTEVENT_TEXT_SIZE];

Byte				gNeedStates[NUM_CONTROL_NEEDS];

Boolean				gEatMouse = false;
Boolean				gMouseMotionNow = false;
static Byte			gMouseButtonState[NUM_SUPPORTED_MOUSE_BUTTONS + 2];

OGLVector2D			gCameraControlDelta;


static OGLVector2D GetThumbStickVector(bool rightStick);


/**********************/
/* STATIC FUNCTIONS   */
/**********************/


static inline void UpdateKeyState(Byte* state, bool downNow)
{
	switch (*state)	// look at prev state
	{
		case KEYSTATE_HELD:
		case KEYSTATE_PRESSED:
			*state = downNow ? KEYSTATE_HELD : KEYSTATE_UP;
			break;
		case KEYSTATE_OFF:
		case KEYSTATE_UP:
		default:
			*state = downNow ? KEYSTATE_PRESSED : KEYSTATE_OFF;
			break;
	}
}

/**********************/
/* IMPLEMENTATION     */
/**********************/

void InitInput(void)
{
}

void UpdateInput(void)
{
	gTextInput[0] = '\0';
	gMouseMotionNow = false;
	gEatMouse = false;

			/**********************/
			/* DO SDL MAINTENANCE */
			/**********************/

	MouseSmoothing_StartFrame();

	int mouseWheelDelta = 0;

	SDL_PumpEvents();
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				ExitToShell();			// throws Pomme::QuitRequest
				return;

			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						ExitToShell();	// throws Pomme::QuitRequest
						return;

					/*
					case SDL_WINDOWEVENT_RESIZED:
						QD3D_OnWindowResized(event.window.data1, event.window.data2);
						break;
					*/

					case SDL_WINDOWEVENT_FOCUS_LOST:
#if __APPLE__
						// On Mac, always restore system mouse accel if cmd-tabbing away from the game
						RestoreMacMouseAcceleration();
#endif
						gEatMouse = true;
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
#if __APPLE__
						// On Mac, kill mouse accel when focus is regained only if the game has captured the mouse
						if (SDL_GetRelativeMouseMode())
							KillMacMouseAcceleration();
#endif
						gEatMouse = true;
						break;
				}
				break;

				case SDL_TEXTINPUT:
					memcpy(gTextInput, event.text.text, sizeof(gTextInput));
					_Static_assert(sizeof(gTextInput) == sizeof(event.text.text), "size mismatch: gTextInput/event.text.text");
					break;

				case SDL_MOUSEMOTION:
					if (!gEatMouse)
					{
						gMouseMotionNow = true;
						MouseSmoothing_OnMouseMotion(&event.motion);
					}
					break;

				case SDL_MOUSEWHEEL:
					if (!gEatMouse)
					{
						mouseWheelDelta += event.wheel.y;
						mouseWheelDelta += event.wheel.x;
					}
					break;

				case SDL_JOYDEVICEADDED:	 // event.jdevice.which is the joy's INDEX (not an instance id!)
					TryOpenController(false);
					break;

				case SDL_JOYDEVICEREMOVED:	// event.jdevice.which is the joy's UNIQUE INSTANCE ID (not an index!)
					OnJoystickRemoved(event.jdevice.which);
					break;

				case SDL_KEYDOWN:
					gUserPrefersGamepad = false;
					break;

				case SDL_CONTROLLERBUTTONDOWN:
				case SDL_CONTROLLERBUTTONUP:
				case SDL_JOYBUTTONDOWN:
					gUserPrefersGamepad = true;
					break;
		}
	}

	// --------------------------------------------
	// Refresh the state of each individual mouse button,
	// including wheelup/wheeldown which we are exposed to the game as buttons

	if (gEatMouse)
	{
		MouseSmoothing_ResetState();
		memset(gMouseButtonState, KEYSTATE_OFF, sizeof(gMouseButtonState));
	}
	else
	{
		uint32_t mouseButtons = SDL_GetMouseState(NULL, NULL);

		// Actual mouse buttons
		for (int i = 1; i < NUM_SUPPORTED_MOUSE_BUTTONS_PURESDL; i++)
		{
			bool downNow = mouseButtons & SDL_BUTTON(i);
			UpdateKeyState(&gMouseButtonState[i], downNow);
		}

		// Wheel up/wheel down fake buttons
		UpdateKeyState(&gMouseButtonState[SDL_BUTTON_WHEELUP],		mouseWheelDelta > 0);
		UpdateKeyState(&gMouseButtonState[SDL_BUTTON_WHEELDOWN],	mouseWheelDelta < 0);
	}

	// --------------------------------------------
	// Refresh the state of each individual keyboard key

	int numkeys = 0;
	const UInt8* keystate = SDL_GetKeyboardState(&numkeys);

	{
		int minNumKeys = numkeys < SDL_NUM_SCANCODES ? numkeys : SDL_NUM_SCANCODES;

		for (int i = 0; i < minNumKeys; i++)
			UpdateKeyState(&gRawKeyboardState[i], keystate[i]);

		// fill out the rest
		for (int i = minNumKeys; i < SDL_NUM_SCANCODES; i++)
			UpdateKeyState(&gRawKeyboardState[i], false);
	}

	// --------------------------------------------

	for (int i = 0; i < NUM_CONTROL_NEEDS; i++)
	{
		const KeyBinding* kb = &gGamePrefs.keys[i];

		bool downNow = false;

		for (int j = 0; j < KEYBINDING_MAX_KEYS; j++)
			if (kb->key[j] && kb->key[j] < numkeys)
				downNow |= 0 != keystate[kb->key[j]];

		downNow |= gMouseButtonState[kb->mouseButton] & KEYSTATE_ACTIVE_BIT;

		if (gSDLController)
		{
			int16_t deadZone = i >= NUM_REMAPPABLE_NEEDS
							   ? kJoystickDeadZone_UI
							   : kJoystickDeadZone;

			for (int j = 0; j < KEYBINDING_MAX_GAMEPAD_BUTTONS; j++)
			{
				switch (kb->gamepad[j].type)
				{
					case kInputTypeButton:
						downNow |= 0 != SDL_GameControllerGetButton(gSDLController, kb->gamepad[j].id);
						break;

					case kInputTypeAxisPlus:
						downNow |= SDL_GameControllerGetAxis(gSDLController, kb->gamepad[j].id) > deadZone;
						break;

					case kInputTypeAxisMinus:
						downNow |= SDL_GameControllerGetAxis(gSDLController, kb->gamepad[j].id) < -deadZone;
						break;

					default:
						break;
				}
			}
		}

		UpdateKeyState(&gNeedStates[i], downNow);
	}


	// -------------------------------------------


			/****************************/
			/* SET PLAYER AXIS CONTROLS */
			/****************************/

	gPlayerInfo.analogControlX = 											// assume no control input
	gPlayerInfo.analogControlZ = 0.0f;

			/* FIRST CHECK ANALOG AXES */

	if (gSDLController)
	{
		OGLVector2D thumbVec = GetThumbStickVector(false);
		if (thumbVec.x != 0 || thumbVec.y != 0)
		{
			gPlayerInfo.analogControlX = thumbVec.x;
			gPlayerInfo.analogControlZ = thumbVec.y;
		}
	}

			/* NEXT CHECK THE DIGITAL KEYS */


	if (GetNeedState(kNeed_TurnLeft))							// is Left Key pressed?
		gPlayerInfo.strafeControlX = -1.0f;
	else
	if (GetNeedState(kNeed_TurnRight))						// is Right Key pressed?
		gPlayerInfo.strafeControlX = 1.0f;
	else
		gPlayerInfo.strafeControlX = 0.0f;			// Make sure not strafing when not pressing strafe


	if (GetNeedState(kNeed_Forward))							// is Up Key pressed?
		gPlayerInfo.analogControlZ = -1.0f;
	else
	if (GetNeedState(kNeed_Backward))						// is Down Key pressed?
		gPlayerInfo.analogControlZ = 1.0f;

		/* AND FINALLY SEE IF MOUSE DELTAS ARE BEST */

	const float mouseSensitivityFrac = (float)gGamePrefs.mouseSensitivityLevel / NUM_MOUSE_SENSITIVITY_LEVELS;
	int mdx, mdy;
	MouseSmoothing_GetDelta(&mdx, &mdy);


	if (gGamePrefs.mouseControlsOtto)
	{
		float mouseDX = mdx * mouseSensitivityFrac * .1f;
		float mouseDY = mdy * mouseSensitivityFrac * .1f;

		// Don't pin the values for better (smoother) mouse fps control
		//mouseDX = GAME_CLAMP(mouseDX, -1.0f, 1.0f);					// keep values pinned
		//mouseDY = GAME_CLAMP(mouseDY, -1.0f, 1.0f);

		if (fabsf(mouseDX) > fabsf(gPlayerInfo.analogControlX))		// is the mouse delta better than what we've got from the other devices?
			gPlayerInfo.analogControlX = mouseDX;
	}


			/* UPDATE SWIVEL CAMERA */

	gCameraControlDelta.x = 0;
	gCameraControlDelta.y = 0;

	if (gSDLController)
	{
		//OGLVector2D rsVec = GetThumbStickVector(true);
		//gCameraControlDelta.x -= rsVec.x * 1.0f;
		//gCameraControlDelta.y += rsVec.y * 1.0f;
	}

	if (GetNeedState(kNeed_CameraLeft))
		gCameraControlDelta.x -= 1.0f;

	if (GetNeedState(kNeed_CameraRight))
		gCameraControlDelta.x += 1.0f;

	if (!gGamePrefs.mouseControlsOtto) 
		gCameraControlDelta.x -= mdx * mouseSensitivityFrac * 0.04f;
	else {
		// While mouse controls Otto (which it always should in FPS), get the camera y stuff here
		gCameraControlDelta.y += mdy * mouseSensitivityFrac * 0.008f;
	}
}

void CaptureMouse(Boolean doCapture)
{
	SDL_PumpEvents();	// Prevent SDL from thinking mouse buttons are stuck as we switch into relative mode
	SDL_SetRelativeMouseMode(doCapture ? SDL_TRUE : SDL_FALSE);
	SDL_ShowCursor(doCapture ? 0 : 1);
//	ClearMouseState();
	EatMouseEvents();

#if __APPLE__
	if (doCapture)
        KillMacMouseAcceleration();
    else
        RestoreMacMouseAcceleration();
#endif
}

void EatMouseEvents(void)
{
	gEatMouse = true;
}

Boolean GetNewKeyState(unsigned short sdlScanCode)
{
	return gRawKeyboardState[sdlScanCode] == KEYSTATE_PRESSED;
}

Boolean GetKeyState(unsigned short sdlScanCode)
{
	return gRawKeyboardState[sdlScanCode] == KEYSTATE_PRESSED || gRawKeyboardState[sdlScanCode] == KEYSTATE_HELD;
}

Boolean UserWantsOut(void)
{
	return GetNewNeedState(kNeed_UIConfirm)
		|| GetNewNeedState(kNeed_UIBack)
		|| GetNewNeedState(kNeed_UIStart)
		|| FlushMouseButtonPress(SDL_BUTTON_LEFT);
}

Boolean GetNeedState(int needID)
{
	GAME_ASSERT(needID < NUM_CONTROL_NEEDS);
	return 0 != (gNeedStates[needID] & KEYSTATE_ACTIVE_BIT);
}

Boolean GetNewNeedState(int needID)
{
	GAME_ASSERT(needID < NUM_CONTROL_NEEDS);
	return gNeedStates[needID] == KEYSTATE_PRESSED;
}

Boolean FlushMouseButtonPress(uint8_t sdlButton)
{
	GAME_ASSERT(sdlButton < NUM_SUPPORTED_MOUSE_BUTTONS);
	Boolean gotPress = KEYSTATE_PRESSED == gMouseButtonState[sdlButton];
	if (gotPress)
		gMouseButtonState[sdlButton] = KEYSTATE_HELD;
	return gotPress;
}

#pragma mark -

/****************************** SDL JOYSTICK FUNCTIONS ********************************/

SDL_GameController* TryOpenController(bool showMessage)
{
	if (gSDLController)
	{
		printf("Already have a valid controller.\n");
		return gSDLController;
	}

	if (SDL_NumJoysticks() == 0)
	{
		return NULL;
	}

	for (int i = 0; gSDLController == NULL && i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			gSDLController = SDL_GameControllerOpen(i);
			gSDLJoystickInstanceID = SDL_JoystickGetDeviceInstanceID(i);
		}
	}

	if (!gSDLController)
	{
		printf("Joystick(s) found, but none is suitable as an SDL_GameController.\n");
		if (showMessage)
		{
			char messageBuf[1024];
			snprintf(messageBuf, sizeof(messageBuf),
					 "The game does not support your controller yet (\"%s\").\n\n"
					 "You can play with the keyboard and mouse instead. Sorry!",
					 SDL_JoystickNameForIndex(0));
			SDL_ShowSimpleMessageBox(
					SDL_MESSAGEBOX_WARNING,
					"Controller not supported",
					messageBuf,
					gSDLWindow);
		}
		return NULL;
	}

	printf("Opened joystick %d as controller: %s\n", gSDLJoystickInstanceID, SDL_GameControllerName(gSDLController));

	return gSDLController;
}

void Rumble(float strength, uint32_t ms)
{
	if (NULL == gSDLController || !gGamePrefs.gamepadRumble)
		return;

#if !(SDL_VERSION_ATLEAST(2,0,9))
	#warning Rumble support requires SDL 2.0.9 or later
#else
	SDL_GameControllerRumble(gSDLController, (Uint16)(strength * 65535), (Uint16)(strength * 65535), ms);
#endif
}

void OnJoystickRemoved(SDL_JoystickID which)
{
	if (NULL == gSDLController)		// don't care, I didn't open any controller
		return;

	if (which != gSDLJoystickInstanceID)	// don't care, this isn't the joystick I'm using
		return;

	printf("Current joystick was removed: %d\n", which);

	// Nuke reference to this controller+joystick
	SDL_GameControllerClose(gSDLController);
	gSDLController = NULL;
	gSDLJoystickInstanceID = -1;

	// Try to open another joystick if any is connected.
	TryOpenController(false);
}

float SnapAngle(float angle, float snap)
{
	if (angle >= -snap && angle <= snap)							// east (-0...0)
		return 0;
	else if (angle >= PI/2 - snap && angle <= PI/2 + snap)			// south
		return PI/2;
	else if (angle >= PI - snap || angle <= -PI + snap)				// west (180...-180)
		return PI;
	else if (angle >= -PI/2 - snap && angle <= -PI/2 + snap)		// north
		return -PI/2;
	else
		return angle;
}

static OGLVector2D GetThumbStickVector(bool rightStick)
{
	Sint16 dxRaw = SDL_GameControllerGetAxis(gSDLController, rightStick ? SDL_CONTROLLER_AXIS_RIGHTX : SDL_CONTROLLER_AXIS_LEFTX);
	Sint16 dyRaw = SDL_GameControllerGetAxis(gSDLController, rightStick ? SDL_CONTROLLER_AXIS_RIGHTY : SDL_CONTROLLER_AXIS_LEFTY);

	float dx = dxRaw / 32767.0f;
	float dy = dyRaw / 32767.0f;

	float magnitudeSquared = dx*dx + dy*dy;

	if (magnitudeSquared < kJoystickDeadZoneFracSquared)
	{
		return (OGLVector2D) { 0, 0 };
	}
	else
	{
		float magnitude;
		
		if (magnitudeSquared > 1.0f)
		{
			// Cap magnitude -- what's returned by the controller actually lies within a square
			magnitude = 1.0f;
		}
		else
		{
			magnitude = sqrtf(magnitudeSquared);

			// Avoid magnitude bump when thumbstick is pushed past dead zone:
			// Bring magnitude from [kJoystickDeadZoneFrac, 1.0] to [0.0, 1.0].
			magnitude = (magnitude - kJoystickDeadZoneFrac) / (1.0f - kJoystickDeadZoneFrac);
		}

		float angle = atan2f(dy, dx);

		angle = SnapAngle(angle, kDefaultSnapAngle);

		return (OGLVector2D) { cosf(angle) * magnitude, sinf(angle) * magnitude };
	}
}
