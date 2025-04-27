#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"
#include <memory>

class Asteroid : public GameObject
{
public:
    Asteroid(); // Default constructor
    ~Asteroid();

    void Update(int t);
    bool CollisionTest(std::shared_ptr<GameObject> o);
    void OnCollision(const GameObjectList& objects);
};

#endif