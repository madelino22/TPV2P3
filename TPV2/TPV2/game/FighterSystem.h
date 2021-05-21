
#pragma once
#include "../ecs/System.h"
#include "../ecs/Entity.h"
#include <vector>
#include "../utils/Vector2D.h"
#include "NetworkSystem.h"
#include "BulletsSystem.h"

class FighterSystem :
    public System
{
public:
    void onCollisionWithAsteroid(Entity* a);
    void init() override;
    void update() override;
    void moveFighter(Transform* tr);
    void setFighterTr(int id, Vector2D pos, int r);
    virtual void receive(const Message&);
private:
    Transform* trIzq;
    Transform* trDr;
    float thrust = 0.2;
    float speedLimit = 3;
    int lives = 3;
    BulletsSystem* bulletsSystem;
};