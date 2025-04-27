#pragma once
#ifndef __SMALLASTEROID_H__
#define __SMALLASTEROID_H__

#include "Asteroid.h"

class SmallAsteroid : public Asteroid
{
public:
    SmallAsteroid(); // Default constructor
    ~SmallAsteroid();

    void Update(int t);
    bool CollisionTest(std::shared_ptr<GameObject> o);
    void OnCollision(const GameObjectList& objects);
};

#endif