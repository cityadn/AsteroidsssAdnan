#include "Asteroid.h"
#include "GameWorld.h"
#include "BoundingShape.h"
#include <cmath>

Asteroid::Asteroid() : GameObject("Asteroid")
{
    mAngle = rand() % 360;
    mRotation = 0;
    mPosition.x = rand() % 800;
    mPosition.y = rand() % 600;
    mPosition.z = 0.0;

    float speedScale = 3.0f;
    mVelocity.x = speedScale * cos(DEG2RAD * mAngle);
    mVelocity.y = speedScale * sin(DEG2RAD * mAngle);
    mVelocity.z = 0.0;
}

Asteroid::~Asteroid() {}

void Asteroid::Update(int t)
{
    mPosition.x += mVelocity.x * t;
    mPosition.y += mVelocity.y * t;

    if (mWorld)
    {
        mWorld->WrapXY(mPosition.x, mPosition.y);
    }
}

bool Asteroid::CollisionTest(std::shared_ptr<GameObject> o)
{
    if (GetType() == o->GetType()) return false;
    if (!mBoundingShape || !o->GetBoundingShape()) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
    mWorld->FlagForRemoval(GetThisPtr());
}