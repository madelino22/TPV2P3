// This file is part of the course TPV2@UCM - Samir Genaim

#include "GameManagerSystem.h"

#include "../components/Transform.h"
#include "../ecs/Manager.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "NetworkSystem.h"


GameManagerSystem::GameManagerSystem() :
		score_(), //
		state_(NEWGAME), //
		maxScore_(3) {
}

GameManagerSystem::~GameManagerSystem() {
}

void GameManagerSystem::init() {
}

void GameManagerSystem::update() {
	if (state_ != RUNNING) {
		if (ih().isKeyDown(SDL_SCANCODE_SPACE)) {
			switch (state_) {
			case NEWGAME:
				startGame();
				break;
			case PAUSED:
				startGame();
				break;
			case GAMEOVER:
				state_ = NEWGAME;
				score_[0] = score_[1] = 0;
				manager_->getSystem<NetworkSystem>()->sendStateChanged(state_,
						score_[0], score_[1]);
				break;
			default:
				break;
			}
		} else if (ih().isKeyDown(SDL_SCANCODE_P)) {
			manager_->getSystem<NetworkSystem>()->switchId();
		}
	}
}

void GameManagerSystem::onFighterDeath(Entity* fighter)
{
	if (manager_->getHandler<LeftFighter>() == fighter)
	{
		score_[1]++;
	}
	else if (manager_->getHandler<RightFighter>() == fighter)
	{
		score_[0]++;
	}
	assert(state_ == RUNNING); // this should be called only when game is runnig

	if (score_[0] < maxScore_ && score_[1] < maxScore_)
		changeState(PAUSED, score_[1], score_[0]);
	else
		changeState(GAMEOVER, score_[1], score_[0]);
	manager_->getSystem<NetworkSystem>()->sendStateChanged(state_, score_[0],score_[1]);
}
void GameManagerSystem::startGame() {
	if (state_ == RUNNING)
		return;

	if (!manager_->getSystem<NetworkSystem>()->isGameReady())
		return;

	auto isMaster = manager_->getSystem<NetworkSystem>()->isMaster();

	if (isMaster) {
		changeState(RUNNING, score_[1], score_[0]);
		//manager_->getSystem<BallSystem>()->initBall();
		manager_->getSystem<NetworkSystem>()->sendStateChanged(state_,
				score_[0], score_[1]);
	} else {
		manager_->getSystem<NetworkSystem>()->sendStartGameRequest();
	}
}

void GameManagerSystem::changeState(Uint8 state, Uint8 left_score,
		Uint8 right_score) {
	state_ = state;
	score_[0] = left_score;
	score_[1] = right_score;
	Transform* trLeft = manager_->getComponent<Transform>(manager_->getHandler<LeftFighter>());
	Transform* trRight = manager_->getComponent<Transform>(manager_->getHandler<RightFighter>());
	trLeft->setPos(Vector2D(sdlutils().width() / 4, sdlutils().height() / 2));
	trRight->setPos(Vector2D(sdlutils().width() * 3 / 4, sdlutils().height() / 2));
	trLeft->setVel(Vector2D(0, 0));
	trRight->setVel(Vector2D(0, 0));
	trLeft->setRot(0);
	trRight->setRot(0);
}

void GameManagerSystem::resetGame() {
	state_ = NEWGAME;
	score_[0] = score_[1] = 0;
	//manager_->getSystem<BallSystem>()->resetBall();
}
