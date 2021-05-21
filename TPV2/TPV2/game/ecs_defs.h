// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include "../game/messages.h"

// components -- forward declaration, i.e., assume they are defined somewhere
struct Transform;
struct Image;
struct PaddleCtrlKeys;

#define _CMPS_LIST_  \
	Transform,\
	Image,\
	PaddleCtrlKeys

// groups
struct Bullets;
#define _GRPS_LIST_ Bullets

// handlers
struct LeftFighter;
struct RightFighter;
#define _HDLRS_LIST_ \
	LeftFighter, \
	RightFighter

// systems
class FighterSystem;
class FighterGunSystem;
class BulletsSystem;
class RenderSystem;
class CollisionSystem;
class GameManagerSystem;
class NetworkSystem;

#define _SYS_LIST_ \
		RenderSystem, \
		FighterSystem, \
		FighterGunSystem, \
		BulletsSystem, \
		CollisionSystem, \
		GameManagerSystem,\
		NetworkSystem
