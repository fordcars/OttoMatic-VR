#pragma once

		/* MY BUILD OPTIONS */

#define __LITTLE_ENDIAN__	1

#if _MSC_VER
	#define _Static_assert static_assert
#endif

#ifdef __cplusplus
extern "C"
{
#endif

		/* HEADERS */

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#include <stdio.h>

#include "Pomme.h"

#include "globals.h"
#include "structs.h"
#include "metaobjects.h"
#include "ogl_support.h"
#include "main.h"
#include "mobjtypes.h"
#include "misc.h"
#include "sound2.h"
#include "sobjtypes.h"
#include "sprites.h"
#include "sparkle.h"
#include "bg3d.h"
#include "camera.h"
#include "collision.h"
#include 	"input.h"
#include "file.h"
#include "window.h"
#include "player.h"
#include "terrain.h"
#include "humans.h"
#include "skeletonobj.h"
#include "skeletonanim.h"
#include "skeletonjoints.h"
#include	"infobar.h"
#include "triggers.h"
#include "effects.h"
#include "shards.h"
#include "bones.h"
#include "vaportrails.h"
#include "splineitems.h"
#include "mytraps.h"
#include "enemy.h"
#include "items.h"
#include "sky.h"
#include "water.h"
#include "fences.h"
#include "miscscreens.h"
#include "objects.h"
#include "mainmenu.h"
#include "lzss.h"
#include "3dmath.h"
#include "ogl_functions.h"
#include "localization.h"
#include "textmesh.h"
#include "tga.h"
#include "menu.h"
#include "vr_support.h"


		/* VR (C Code only) */
//Gets action manifest and action handles
void vrcpp_initSteamVRInput(void);

// Reads the current state into all actions (call each frame)
// After this call, the results of Get*ActionData calls will be the same until the next call to UpdateActionState
void vrcpp_UpdateActionState(void);

// Check if a IVRInput button is pressed
// actionToDo  ->   the button (enum)
// preventPressAndHold  ->   prevent old actions, should be false for most game actions but true for most menus
bool vrcpp_GetDigitalActionData(int actionToDo, bool preventPressAndHold);

// Get Action Data (see if triggered). This function is for analog (vector) actions only, no digital bools
vrJoyPos vrcpp_GetAnalogActionData(int actionToDo);

// HAPTIC TRIGGER
//handToVibrate -> Wether to vibrate both hands or just 1
//fStartSecondsFromNow -> When to start the haptic event
//fDurationSeconds -> How long to trigger the haptic event for
//fFrequency -> The frequency in cycles per second of the haptic event
//fAmplitude -> The magnitude of the haptic event.This value must be between 0.0 and 1.0.
void vrcpp_DoVibrationHaptics(int handToVibrate,
	float fDurationSeconds, float fFrequency, float fAmplitude);


void updateHMDposition(void);
void updateGameSpacePositions(); // Updates the gameSpace coordinate members within the vrInfo Structs



		/* EXTERNS */
extern	BG3DFileContainer		*gBG3DContainerList[];
extern  Boolean                 gInitVRYawAlignDone;
extern	Boolean					gAllowAudioKeys;
extern	Boolean					gAutoRotateCamera;
extern	Boolean					gBrainBossDead;
extern	Boolean					gBumperCarGateBlown[];
extern	Boolean					gDisableAnimSounds;
extern	Boolean					gDisableHiccupTimer;
extern	Boolean					gDoDeathExit;
extern  Boolean                 gPlayerInMainMenu;
extern	Boolean					gDoJumpJetAtApex;
extern	Boolean					gDrawLensFlare;
extern	Boolean					gExplodePlayerAfterElectrocute;
extern	Boolean					gForceCameraAlignment;
extern	Boolean					gFreezeCameraFromXZ;
extern	Boolean					gFreezeCameraFromY;
extern	Boolean					gG4;
extern	Boolean					gGameOver;
extern	Boolean					gGamePaused;
extern  Boolean					gGameIsPausedVR;
extern	Boolean					gHelpMessageDisabled[NUM_HELP_MESSAGES];
extern	Boolean					gIceCracked;
extern	Boolean					gLevelCompleted;
extern	Boolean					gMouseMotionNow;
extern	Boolean					gMyState_Lighting;
extern	Boolean					gPlayerFellIntoBottomlessPit;
extern	Boolean					gPlayerHasLanded;
extern	Boolean					gPlayerIsDead;
extern	Boolean					gPlayingFromSavedGame;
extern	Boolean					gUserPrefersGamepad;
extern	Byte					**gMapSplitMode;
extern	Byte					gDebugMode;
extern	Byte					gHumansInSaucerList[];
extern	ChannelInfoType			gChannelInfo[];
extern	CollisionBoxType 		*gSaucerIceBounds;
extern	CollisionRec			gCollisionList[];
extern	CommandLineOptions		gCommandLine;
extern	FSSpec					gDataSpec;
extern	FenceDefType			*gFenceList;
extern	HighScoreType			gHighScores[];
extern	MOMaterialObject		*gMostRecentMaterial;
extern	MOMaterialObject		*gSuperTileTextureObjects[MAX_SUPERTILE_TEXTURES];
extern	MOVertexArrayData		**gLocalTriMeshesOfSkelType;
extern	MetaObjectPtr			gBG3DGroupList[MAX_BG3D_GROUPS][MAX_OBJECTS_IN_GROUP];
extern	NewObjectDefinitionType	gNewObjectDefinition;
extern	NewParticleGroupDefType	gNewParticleGroupDef;
extern	OGLBoundingBox			gObjectGroupBBoxList[MAX_BG3D_GROUPS][MAX_OBJECTS_IN_GROUP];
extern	OGLBoundingBox			gWaterBBox[];
extern	OGLColorRGB				gGlobalColorFilter;
extern	OGLMatrix4x4			*gCurrentObjMatrix;
extern	OGLMatrix4x4			gViewToFrustumMatrix;
extern	OGLMatrix4x4			gWorldToFrustumMatrix;
extern	OGLMatrix4x4			gWorldToViewMatrix;
extern	OGLMatrix4x4			gWorldToWindowMatrix;
extern	OGLPoint2D				gBestCheckpointCoord;
extern	OGLPoint3D				gCoord;
extern	OGLSetupOutputType		*gGameViewInfoPtr;
extern	OGLVector2D				gCameraControlDelta;
extern	OGLVector3D				gDelta;
extern	OGLVector3D				gRecentTerrainNormal;
extern	OGLVector3D				gWorldSunDirection;
extern	ObjNode					*gAlienSaucer;
extern	ObjNode					*gCurrentNode;
extern	ObjNode					*gCurrentZip;
extern	ObjNode					*gExitRocket;
extern	ObjNode					*gFirstNodePtr;
extern	ObjNode					*gMagnetMonsterList[MAX_MAGNET_MONSTERS];
extern	ObjNode					*gPlayerRocketSled;
extern	ObjNode					*gPlayerSaucer;
extern	ObjNode					*gSaucerTarget;
extern	ObjNode					*gSoapBubble;
extern	ObjNode					*gTargetPickup;
extern	ObjNode					*gTractorBeamObj;
extern	ParticleGroupType		*gParticleGroups[];
extern	PrefsType				gGamePrefs;
extern	SDL_GameController		*gSDLController;
extern	SDL_GLContext			gAGLContext;
extern	SDL_Window				*gSDLWindow;
extern	SparkleType				gSparkles[MAX_SPARKLES];
extern	SplineDefType			**gSplineList;
extern	SpriteType				*gSpriteGroupList[MAX_SPRITE_GROUPS];
extern	SuperTileGridType		**gSuperTileTextureGrid;
extern	SuperTileItemIndexType	**gSuperTileItemIndexGrid;
extern	SuperTileMemoryType		gSuperTileMemoryList[];
extern	SuperTileStatus			**gSuperTileStatusGrid;
extern	TerrainItemEntryType	**gMasterItemList;
extern	TileAttribType			**gTileAttribList;
extern	WaterDefType			**gWaterListHandle;
extern	WaterDefType			*gWaterList;
extern	char					gTextInput[SDL_TEXTINPUTEVENT_TEXT_SIZE];
extern	const KeyBinding		kDefaultKeyBindings[NUM_CONTROL_NEEDS];
extern	const MenuStyle			kDefaultMenuStyle;
extern	const OGLPoint3D		gPlayerMuzzleTipOff;
extern  OGLPoint3D			    gVrHMDPosMovedeltaWorldspace;
extern	const int				kLevelSoundBanks[NUM_LEVELS];
extern	float					gSkyAltitudeY;
extern	float					**gMapYCoords;
extern	float					**gMapYCoordsOriginal;
extern	float					g2DLogicalHeight;
extern	float					g2DLogicalWidth;
extern	float					gAutoFadeEndDist;
extern	float					gAutoFadeRange_Frac;
extern	float					gAutoFadeStartDist;
extern	float					gAutoRotateCameraSpeed;
extern	float					gBeamCharge;
extern	float					gBestCheckpointAim;
extern	float					gCameraDistFromMe;
extern	float					gCameraLookAtYOff;
extern	float					gCameraUserRotY;
extern	float					gCurrentAspectRatio;
extern	float					gCurrentMaxSpeed;
extern	float					gDeathTimer;
extern	float					gDischargeTimer;
extern	float					gFramesPerSecond;
extern	float					gFramesPerSecondFrac;
extern	float					gGlobalTransparency;
extern	float					gGravity;
extern	float					gHumanScaleRatio;
extern	float					gJumpJetWarningCooldown;
extern	float					gLevelCompletedCoolDownTimer;
extern	float					gMinHeightOffGround;
extern	float					gPlayerBottomOff;
extern	float					gPlayerToCameraAngle;
extern	float					gRocketScaleAdjust;
extern	float					gSpinningPlatformRot;
extern	float					gTargetMaxSpeed;
extern	float					gTileSlipperyFactor;
extern	float					gTimeSinceLastShoot;
extern	float					gTimeSinceLastThrust;
extern	int						gGameWindowHeight;
extern	int						gGameWindowWidth;
extern	int						gLevelNum;
extern	int						gNumHumansInTransit;
extern	int						gNumHumansRescuedTotal;
extern	int						gNumIceCracks;
extern	int						gNumObjectNodes;
extern	int						gNumObjectsInBG3DGroupList[MAX_BG3D_GROUPS];
extern	int						gNumSparkles;
extern	int						gPolysThisFrame;
extern	int						gVRAMUsedThisFrame;
extern	long					gNumFences;
extern	long					gNumSplines;
extern	long					gNumSpritesInGroupList[MAX_SPRITE_GROUPS];
extern	long					gNumSuperTilesDeep;
extern	long					gNumSuperTilesWide;
extern	long					gNumUniqueSuperTiles;
extern	long					gNumWaterPatches;
extern	long					gPrefsFolderDirID;
extern	long					gTerrainTileDepth;
extern	long					gTerrainTileWidth;
extern	long					gTerrainUnitDepth;
extern	long					gTerrainUnitWidth;
extern	short					gBeamMode;
extern	short					gBeamModeSelected;
extern	short					gBestCheckpointNum;
extern	short					gDisplayedHelpMessage;
extern	short					gMainAppRezFile;
extern	short					gNumActiveParticleGroups;
extern	short					gNumCollisions;
extern	short					gNumEnemies;
extern	short					gNumFencesDrawn;
extern	short					gNumHumansInSaucer;
extern	short					gNumHumansRescuedOfType[NUM_HUMAN_TYPES];
extern	short					gNumSuperTilesDrawn;
extern	short					gNumTerrainDeformations;
extern	short					gNumTerrainItems;
extern	short					gNumWaterDrawn;
extern	short					gPrefsFolderVRefNum;
extern	signed char				gNumEnemyOfKind[];
extern	u_long					gAutoFadeStatusBits;
extern	u_long					gGameFrameNum;
extern	u_long					gGlobalMaterialFlags;
extern	u_long					gLoadedScore;
extern	u_long					gScore;
extern	u_short					**gTileGrid;
extern	u_short					gTileAttribFlags;
#ifdef __cplusplus
};
#endif

#define GAME_ASSERT(condition) do { if (!(condition)) DoAssert(#condition, __func__, __LINE__); } while(0)
#define GAME_ASSERT_MESSAGE(condition, message) do { if (!(condition)) DoAssert(message, __func__, __LINE__); } while(0)
#define DECLARE_WORKBUF(buf, bufSize) char (buf)[256]; const int (bufSize) = 256
#define DECLARE_STATIC_WORKBUF(buf, bufSize) static char (buf)[256]; static const int (bufSize) = 256
