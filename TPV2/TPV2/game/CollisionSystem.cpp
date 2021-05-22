// This file is part of the course TPV2@UCM - Samir Genaim

#include "CollisionSystem.h"

#include <cassert>
#include "../components/Transform.h"
#include "../ecs/Manager.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Collisions.h"
#include "GameManagerSystem.h"
#include "NetworkSystem.h"

CollisionSystem::CollisionSystem() 
{

}


CollisionSystem::~CollisionSystem() {
}

void CollisionSystem::init() {
	
}
void CollisionSystem::update() {

	if (!manager_->getSystem<NetworkSystem>()->isMaster())
		return;

	if (manager_->getSystem<GameManagerSystem>()->getState() != GameManagerSystem::RUNNING)
		return;
        int n = (*entidades).size();
        //Se recorre la lista de entidades
        for (int i = 0; i < n; ++i)
            //para cada entidad del grupoi asteroides
            if (manager_->getHandler<LeftFighter>() == (*entidades)[i] || manager_->getHandler<RightFighter>() == (*entidades)[i])
            {
                auto aTR = manager_->getComponent<Transform>((*entidades)[i]); // Transform del caza
                for (int j = 0; j < n; ++j)
                    //se recorre otra vez la lista preguntando a las entidades del grupo balas o a la nave
                    if (manager_->hasGroup<Bullets>((*entidades)[j])) // Choque con las balas
                    {
                        auto bTR = manager_->getComponent<Transform>((*entidades)[j]); // Transform de la bala
                        //si han chocado
                        if (Collisions::collidesWithRotation(aTR->getPos(), aTR->getW(), aTR->getH(), aTR->getRot(),
                            bTR->getPos(), bTR->getW(), bTR->getH(), bTR->getRot()))
                        {
                            manager_->getSystem<GameManagerSystem>()->onFighterDeath((*entidades)[i]);
                           
                            /*Message astBullColl;
                            astBullColl.id_ = ASTEROID_COLLISION_WITHBULLET;
                            astBullColl.entitiesCol.asteroid = (entidades)[i];
                            astBullColl.entitiesCol.bullet = (entidades)[j];
                            manager->send(astBullColl);*/

                        }
                    }
            }           
}


