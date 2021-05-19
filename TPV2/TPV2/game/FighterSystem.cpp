#include "FighterSystem.h"
#include "../ecs/Manager.h"
#include "../sdlutils/SDLUtils.h"
#include "../components/Transform.h"
#include "../sdlutils/InputHandler.h"
#include "GameManagerSystem.h"
#include "NetworkSystem.h"

void FighterSystem::onCollisionWithAsteroid(Entity* a)
{
	//// El jugador pierde una vida
	//lives--;

	//// El caza vuelve al centro de la pantalla con velocidad nula y orientado hacia arriba
	//tr->setRot(0.0f);
	//tr->setVel(Vector2D(0, 0));
	//tr->setPos(Vector2D(sdlutils().width() / 2.0f, sdlutils().height() / 2.0f));
	//// Se eliminan los componentes de accion del caza para que no se pueda mover ni disparar hasta que comience la partida
	//
	////Message jet_destroyed;
	////jet_destroyed.id_ = JET_DESTROYED;
	////jet_destroyed.jetDest.lives = lives;
	////manager_->send(jet_destroyed);
	////
	//
	//


	//manager_->setActive(a, false);
}





void FighterSystem::init()
{
	//esto iría en el fighterSystem
	Entity* fighterIzq = manager_->addEntity();
	manager_->addComponent<Transform>(fighterIzq, Vector2D(sdlutils().width() / 4, sdlutils().height() / 2), Vector2D(0, 0), 20, 20, 0);
	manager_->setHandler<LeftFighter>(fighterIzq);
	//manager_->setGroup<RightFighter>(fighter,true);*/
	trIzq = manager_->getComponent<Transform>(fighterIzq);
	/*bulletsSystem = manager_->getSystem<BulletsSystem>();*/


	Entity* fighterDr = manager_->addEntity();
	manager_->addComponent<Transform>(fighterDr, Vector2D(sdlutils().width() * 3 / 4, sdlutils().height() / 2), Vector2D(0, 0), 20, 20, 0);
	manager_->setHandler<RightFighter>(fighterDr);
	//manager_->setGroup<JET>(fighter,true);*/
	trDr = manager_->getComponent<Transform>(fighterDr);
	/*bulletsSystem = manager_->getSystem<BulletsSystem>();*/
}
void FighterSystem::update()
{
	if (manager_->getSystem<GameManagerSystem>()->getState() == GameManagerSystem::RUNNING)
	{
		Uint8 myId = manager_->getSystem<NetworkSystem>()->getId();
		if (myId == 0) {
			moveFighter(trIzq);
		}
		else {
			moveFighter(trDr);
		}

	

	}
}

void FighterSystem::setFighterTr(int id, Vector2D pos, int r)
{
	if (id == 0) {
		trIzq->setPos(pos);
		trIzq->setRot(r);
	}
	else {
		trDr->setPos(pos);
		trDr->setRot(r);

	}
}


void FighterSystem::moveFighter(Transform* tr)
{

		auto& pos = tr->getPos();
		auto& vel = tr->getVel();
		auto r = tr->getRot();
		auto w = tr->getW();
		auto h = tr->getH();
		pos = pos + vel; // Esto lo hacía el update antes del transform
		if (pos.getX() + w / 2 < 0) {
			pos.setX(sdlutils().width() - w / 2);
		}
		else if (pos.getX() + w / 2 > sdlutils().width()) {
			pos.setX(-w / 2);
		}

		if (pos.getY() + h / 2 < 0) {
			pos.setY(sdlutils().height() - h / 2);
		}
		else if (pos.getY() + h / 2 > sdlutils().height()) {
			pos.setY(-h / 2);
		}
		if (ih().keyDownEvent()) {

			//Si se presiona la s, acelera
			if (ih().isKeyDown(SDL_SCANCODE_UP)) {

				Vector2D newVel;
				r = tr->getRot();
				auto& vel = tr->getVel();
				//se acelera en la dirección en la que se es´ta mirando por lo que
				//hay que tener en cuenta  la rotación
				newVel = vel + Vector2D(0, -1).rotate(r) * thrust;

				//limitador de velocidad
				if (newVel.magnitude() > speedLimit)
					newVel = newVel.normalize() * speedLimit;


				(vel).setX(newVel.getX());
				(vel).setY(newVel.getY());

				//además, al acelerar se reproduce un sonido
				sdlutils().soundEffects().at("thrust").play();

			}


			//si se gira izquierda o derecha se rota la nave
			if (ih().isKeyDown(SDL_SCANCODE_LEFT)) {
				tr->setRot(tr->getRot() - 5.0f);
			}
			else if (ih().isKeyDown(SDL_SCANCODE_RIGHT)) {
				tr->setRot(tr->getRot() + 5.0f);
			}

		}
		vel = vel * 0.995f;

		manager_->getSystem<NetworkSystem>()->sendFighterPos(tr->pos_, tr->getRot());

}



void FighterSystem::receive(const Message& m)
{
	//if (m.id_ == JET_COLLISION_WITH_ASTEROID) onCollisionWithAsteroid(m.jetCol.entity);
	//else if (m.id_ == STATE_CHANGED) {
	//	if (m.state_changed.state == NEWGAME) lives = 3;
	//}
}
