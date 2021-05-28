// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include "../ecs/System.h"
#include "../ecs/Entity.h"
#include <vector>
struct Transform;
class SoundEffect;

class CollisionSystem: public System {
public:
	
	void update() override;
	void getEntities(const std::vector<Entity*>* ent) { entidades = ent; };
private:
	const std::vector<Entity*>* entidades;
};

