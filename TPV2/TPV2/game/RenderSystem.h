// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <SDL_pixels.h>

#include "../ecs/System.h"
#include <vector>
#include "../ecs/Entity.h"
#include "../sdlutils/Texture.h"


struct Transform;

class RenderSystem: public System {
protected:


	const std::vector<Entity*>* entidades;


public:
	RenderSystem();
	virtual ~RenderSystem();
	void init() override;
	void update() override;
	void getEntities(const std::vector<Entity*>* ent) { entidades = ent; }

private:

	void drawScore();
	void drawMsgs();
	void drawNames();

	Texture* naveIzq;
	Texture*  naveDr;
	Texture*  balaTex;
	SDL_Rect src_;

	Transform* tr_;
	Transform *leftFighter;
	Transform *rightFighter;
};

