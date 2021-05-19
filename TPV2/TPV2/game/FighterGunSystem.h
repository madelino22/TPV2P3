#pragma once
#include "../ecs_4/ecs/System.h"
#include "../ecs_4/ecs/Entity.h"
#include <vector>
#include "../utils/Vector2D.h"
#include "../sdlutils/InputHandler.h"
#include "../components/Transform.h"
#include "BulletsSystem.h"
class FighterGunSystem : public System
{
private:
	int timer = 0;
	BulletsSystem* bulletsSystem;
	Transform* tr;
public:
	void update();
	void init();
};