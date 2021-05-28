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

void GameManagerSystem::destruyeBalas()
{
	const std::vector<Entity*>& entidades = manager_->getEnteties();
	for (Entity* e : entidades) {
		if (manager_->hasGroup<Bullets>(e)) {
			manager_->setActive(e,false);
		}
	}
}

void GameManagerSystem::onFighterDeath(Entity* fighter)
{
	//al meterse en este método siendo el cliente solo reproducirá el sonido

	//si se es el master se hace todo y se le envía  la info al cliente, si es el cliente solo se 
	//reproduce el sonido, se va a cambiar el estado al recibir el mensaje del master

	auto isMaster = manager_->getSystem<NetworkSystem>()->isMaster();
	if (isMaster) {
		if (manager_->getHandler<LeftFighter>() == fighter)
		{
			score_[1]++;
		}
		else if (manager_->getHandler<RightFighter>() == fighter)
		{
			score_[0]++;
		}
		assert(state_ == RUNNING); // this should be called only when game is runnig


		//Se cambia al estado correspondiente
		if (score_[0] < maxScore_ && score_[1] < maxScore_)
			changeState(PAUSED, score_[0], score_[1]);
		else
			changeState(GAMEOVER, score_[0], score_[1]);


		manager_->getSystem<NetworkSystem>()->tryDestroy();
		//se envía un mensaje al cliente diciendole el nuevo estado y las puntuaciones
		manager_->getSystem<NetworkSystem>()->sendStateChanged(state_, score_[0], score_[1]);
	}


	sdlutils().soundEffects().at("bangSmall").play();

	
}
void GameManagerSystem::startGame() {
	if (state_ == RUNNING)
		return;

	if (!manager_->getSystem<NetworkSystem>()->isGameReady())
		return;

	auto isMaster = manager_->getSystem<NetworkSystem>()->isMaster();

	if (isMaster) {
		//si es el master el que le ha dado al start, empieza la partida y se le enví aun mensaje al cliente
		changeState(RUNNING, score_[0], score_[1]);
		manager_->getSystem<NetworkSystem>()->sendStateChanged(state_,
				score_[0], score_[1]);
	} else {
		//si es el cliente el que le dió al espacio para empezar le envía un mensaje dicinedo que quiere empezar al
		//master, y será el master el que empiece y le envíe después otro mensaje diciendo que ha empezado
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
	
	
	//reinicia los valores por defecto de los cazas
	trLeft->setPos(Vector2D(sdlutils().width() / 4, sdlutils().height() / 2));
	trRight->setPos(Vector2D(sdlutils().width() * 3 / 4, sdlutils().height() / 2));
	trLeft->setVel(Vector2D(0, 0));
	trRight->setVel(Vector2D(0, 0));
	trLeft->setRot(0);
	trRight->setRot(0);
	
	//destruye las entidades de las balas
	destruyeBalas();
}

void GameManagerSystem::resetGame() {
	state_ = NEWGAME;
	score_[0] = score_[1] = 0;
}
