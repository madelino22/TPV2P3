#include "BulletsSystem.h"
#include "../ecs_4/ecs/Manager.h"
#include "../game/FighterSystem.h"
#include "../sdlutils/SDLUtils.h"



void BulletsSystem::onCollisionWithAsteroid(Entity* b, Entity* a)
{
	//lo único que hace es destruir la bala
	manager_->setActive(b, false);
}

void BulletsSystem::shoot(Vector2D pos, Vector2D vel, double width, double height)
{
	//al disparar se crea una nueva entidad que es la bala
	Entity* balaTest = manager_->addEntity();
	//la ala utilizará los datos del transform de la nave
	Vector2D p = tr_->getPos();
	Vector2D velJet = tr_->getVel();
	float w = tr_->getW();
	float h = tr_->getH();
	float r = tr_->getRot();
		
		//vel.angle(Vector2D(0.0f, -1.0f));

	//se calcula, teniendo en cuenta la posición y rotación de la nave,
	//dónde va a salir bala, la rotación y la velocidad
	Vector2D bPos = p + Vector2D(w / 2.0f, h / 2.0f) - Vector2D(0.0f, h / 2.0f + 5.0f + 12.0f).rotate(r) - Vector2D(2.0f, 10.0f);
	Vector2D bVel = Vector2D(0.0f, -1.0f).rotate(r) * (velJet.magnitude() + 5.0f);
	manager_->addComponent<Transform>(balaTest, bPos, bVel, 5.0f, 20.0f, r);
	//Se le añade al grupo de la s balas
	manager_->setGroup<Bullets>(balaTest, true);
		//reproduce un soinido al ser disparada
		sdlutils().soundEffects().at("fire").play();

}
void BulletsSystem::update()
{
	for (Entity* e : *entidades)
	{
		if (manager_->hasGroup<Bullets>(e))
		{
			Transform* tr_bala = manager_->getComponent<Transform>(e);
			auto& pos = tr_bala->getPos();
			
			auto w = tr_bala->getW();
			auto h = tr_bala->getH();

			pos = pos + tr_bala->getVel();
			//si el transform de la entidad muestra que se ha salido de la pantalla, se elimina el objeto
			if (pos.getX() + w / 2 < 0 || pos.getX() + w / 2 > sdlutils().width() || pos.getY() + h / 2 < 0 || pos.getY() + h / 2 > sdlutils().height()) {
				manager_->setActive(e,false);
			}
		}
	}
}

void BulletsSystem::init()
{

	tr_ = manager_->getComponent<Transform>(manager_->getHandler<JET>());
}

void BulletsSystem::receive(const Message& m)
{
	if (m.id_ == ASTEROID_COLLISION_WITH_BULLET)
		onCollisionWithAsteroid(m.entitiesCol.bullet, m.entitiesCol.asteroid);
}
