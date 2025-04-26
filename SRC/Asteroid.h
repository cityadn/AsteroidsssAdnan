#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"
#include <memory>

class Asteroid : public GameObject
{
public:
    Asteroid();
    virtual ~Asteroid();

    void Update(int t) override;
    bool CollisionTest(std::shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;
};

#endif