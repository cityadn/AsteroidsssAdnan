#pragma once
#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "GameObject.h"
#include <memory>

class PowerUp : public GameObject
{
public:
    PowerUp(); // Default constructor
    virtual ~PowerUp();

    virtual void Update(int t);
    bool CollisionTest(std::shared_ptr<GameObject> o);
    virtual void OnCollision(const GameObjectList& objects);
};

#endif