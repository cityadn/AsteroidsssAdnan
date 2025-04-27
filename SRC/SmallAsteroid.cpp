#include "SmallAsteroid.h"
#include "GameWorld.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include <cmath>
#include <iostream>
#include "GameUtil.h"
#include "GameObject.h"

SmallAsteroid::SmallAsteroid() : Asteroid()
{
    mType = GameObjectType("SmallAsteroid");
    mVelocity = mVelocity * 1.5f;
    mScale = 0.1f;
}

SmallAsteroid::~SmallAsteroid() {}

void SmallAsteroid::Update(int t)
{
    mPosition.x += mVelocity.x * t;
    mPosition.y += mVelocity.y * t;

    if (mWorld)
    {
        mWorld->WrapXY(mPosition.x, mPosition.y);
    }
}

bool SmallAsteroid::CollisionTest(std::shared_ptr<GameObject> o)
{
    if (!mBoundingShape || !o->GetBoundingShape()) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void SmallAsteroid::OnCollision(const GameObjectList& objects)
{
    for (auto& object : objects)
    {
		if (object->GetType().GetTypeName() == "Asteroid" || object->GetType().GetTypeName() == "SmallAsteroid")
		{
            shared_ptr<Asteroid> otherAsteroid = std::dynamic_pointer_cast<Asteroid>(object);
        }
        if (object->GetType().GetTypeName() == "Bullet")
        {
            mWorld->FlagForRemoval(GetThisPtr());
        }
    }
}