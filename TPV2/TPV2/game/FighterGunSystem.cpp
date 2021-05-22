#include "FighterGunSystem.h"
#include "GameManagerSystem.h"
#include "NetworkSystem.h"
#include "../ecs/Manager.h"
#include "../sdlutils/SDLUtils.h"
void FighterGunSystem::init()
{
	
	bulletsSystem = manager_->getSystem<BulletsSystem>();

}
void FighterGunSystem::update()
{
	if (manager_->getSystem<GameManagerSystem>()->getState() == GameManagerSystem::RUNNING)
	{
	
	
		if (ih().keyDownEvent()) {

			//Si se presiona la s, dispara
			if (ih().keyDownEvent()) {
				if (ih().isKeyDown(SDLK_s) && sdlutils().currRealTime() >= timer + 250) {

					//Cada vez que dispare hayq eu ver la id ya que en el init no vale ya qeu se peude cambiar la posicion del jugador des
					if (manager_->getSystem<NetworkSystem>()->getId() == 0)tr = manager_->getComponent<Transform>(manager_->getHandler<LeftFighter>());
					else tr = manager_->getComponent<Transform>(manager_->getHandler<RightFighter>());

					auto& pos = tr->getPos();
					auto& vel = tr->getVel();
					auto r = tr->getRot();


					timer = sdlutils().currRealTime();
					bulletsSystem->shoot(pos, vel, 50, 50, tr);
					manager_->getSystem<NetworkSystem>()->tryShoot();
				}
			}
		}
	}
}