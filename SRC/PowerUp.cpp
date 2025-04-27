#include "PowerUp.h"
#include "GameWorld.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include <cmath>
#include <iostream>

// Default constructor for PowerUp
PowerUp::PowerUp() : GameObject("ship_upgrade")
{
    mAngle = rand() % 360;
    mRotation = 0;
    mPosition.x = rand() % 800;
    mPosition.y = rand() % 600;
    mPosition.z = 0.0;

    mVelocity.x = 0.0;
    mVelocity.y = 0.0;
    mVelocity.z = 0.0;
}

// Destructor for PowerUp
PowerUp::~PowerUp() {}

void PowerUp::Update(int t)
{
    if (mWorld)
    {
        mWorld->WrapXY(mPosition.x, mPosition.y);
    }
}

bool PowerUp::CollisionTest(std::shared_ptr<GameObject> o)
{
    if (!mBoundingShape || !o->GetBoundingShape()) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void PowerUp::OnCollision(const GameObjectList& objects)
{
    for (auto& obj : objects)
        if (obj->GetType() == GameObjectType("Spaceship"))
        {
            mWorld->FlagForRemoval(GetThisPtr());
        }
}
