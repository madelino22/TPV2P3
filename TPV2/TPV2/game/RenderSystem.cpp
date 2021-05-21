// This file is part of the course TPV2@UCM - Samir Genaim

#include "RenderSystem.h"

#include "../components/Image.h"
#include "../components/Transform.h"
#include "../ecs/Manager.h"
#include "../sdlutils/macros.h"
#include "../sdlutils/SDLUtils.h"
#include "GameManagerSystem.h"
#include "NetworkSystem.h"

RenderSystem::RenderSystem() : 
		leftFighter(nullptr), //
		rightFighter(nullptr) {
	naveIzq = &sdlutils().images().at("nave");
	naveDr = &sdlutils().images().at("nave2");
	balaTex = &sdlutils().images().at("fire");
}

RenderSystem::~RenderSystem() {
}

void RenderSystem::init() {
	
	leftFighter = manager_->getComponent<Transform>(manager_->getHandler<LeftFighter>());
	assert(leftFighter != nullptr);
	rightFighter = manager_->getComponent<Transform>(manager_->getHandler<RightFighter>());
	assert(rightFighter != nullptr);
	

	
}

void RenderSystem::update() {
	for (Entity* e : *entidades)
	{
		tr_ = manager_->getComponent<Transform>(e);
		SDL_Rect dest = build_sdlrect(tr_->getPos(), tr_->getW(), tr_->getH()); // Rectangulo destino donde se va a dibujar el objeto en pantalla
		if (tr_ == leftFighter)
		{
			src_ = { 0,0, naveIzq->width(), naveIzq->height() };
			naveIzq->render(src_, dest, tr_->getRot());
		}
		else if (tr_ == rightFighter) {
			src_ = { 0,0, naveDr->width(), naveDr->height() };
			naveDr->render(src_, dest, tr_->getRot());
		}
		else if (manager_->hasGroup<Bullets>(e))
		{
			src_ = { 0,0, balaTex->width(), balaTex->height() };
			balaTex->render(src_, dest, tr_->getRot());
		}
		
	}

	drawScore();
	drawMsgs();
	drawNames();
}



void RenderSystem::drawScore() {

	auto &score_ = manager_->getSystem<GameManagerSystem>()->getScore();
	// score
	Texture scoreMsg(
			sdlutils().renderer(), //
			std::to_string(score_[0]) + " - " + std::to_string(score_[1]),
			sdlutils().fonts().at("ARIAL16"), build_sdlcolor(0xffffffff));
	scoreMsg.render((sdlutils().width() - scoreMsg.width()) / 2, 10);
}

void RenderSystem::drawMsgs() {
	auto state_ = manager_->getSystem<GameManagerSystem>()->getState();
	// message when game is not running
	if (state_ != GameManagerSystem::RUNNING) {

		// game over message
		if (state_ == GameManagerSystem::GAMEOVER) {
			auto &t = sdlutils().msgs().at("gameover");
			t.render((sdlutils().width() - t.width()) / 2,
					(sdlutils().height() - t.height()) / 2 - 50);
		}

		// new game message
		if (state_ == GameManagerSystem::NEWGAME) {
			auto &t = sdlutils().msgs().at("start");
			t.render((sdlutils().width() - t.width()) / 2,
					sdlutils().height() / 2 + t.height() * 2);
		} else {
			auto &t = sdlutils().msgs().at("continue");
			t.render((sdlutils().width() - t.width()) / 2,
					sdlutils().height() / 2 + t.height() * 2);
		}

		auto myId = manager_->getSystem<NetworkSystem>()->getId();

		// draw player side message
		Texture side(
				sdlutils().renderer(), //
				(myId == 0 ? "You are playing left" : " You are playing right"),
				sdlutils().fonts().at("ARIAL16"), build_sdlcolor(0xffffffff));
		side.render((sdlutils().width() - side.width()) / 2,
				sdlutils().height() - side.height() - 2 * side.height() - 10);

		// draw switch side message
		auto isMaster = manager_->getSystem<NetworkSystem>()->isMaster();
		auto isGameReady = manager_->getSystem<NetworkSystem>()->isGameReady();

		if (isMaster && !isGameReady) {
			Texture switch_key(
					sdlutils().renderer(), //
					"(press P to switch side)",
					sdlutils().fonts().at("ARIAL16"),
					build_sdlcolor(0xffffffff));
			switch_key.render((sdlutils().width() - switch_key.width()) / 2,
					sdlutils().height() - switch_key.height() - 10);
		}

	}
}

void RenderSystem::drawNames() {
	auto &names_ = manager_->getSystem<NetworkSystem>()->getNames();

	// name of player 0
	Texture name_0(
			sdlutils().renderer(), //
			names_[0], sdlutils().fonts().at("ARIAL16"),
			build_sdlcolor(0xffffffff));
	name_0.render(10, 10);

	// name of player 1
	Texture name_1(
			sdlutils().renderer(), //
			names_[1], sdlutils().fonts().at("ARIAL16"),
			build_sdlcolor(0xffffffff));
	name_1.render(sdlutils().width() - name_1.width() - 10, 10);

	// draw a star next to the master name
	auto isMaster = manager_->getSystem<NetworkSystem>()->isMaster();
	auto myId = manager_->getSystem<NetworkSystem>()->getId();

	Texture isMasterMsg(sdlutils().renderer(), //
			"*", //
			sdlutils().fonts().at("ARIAL24"), build_sdlcolor(0xff0000ff));
	isMasterMsg.render(
			(isMaster && myId == 1) || (!isMaster && myId == 0) ?
					sdlutils().width() - isMasterMsg.width() - 1 : 1, 10);

}
