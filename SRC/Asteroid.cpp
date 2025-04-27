#include "Asteroid.h"
#include "SmallAsteroid.h"
#include "GameWorld.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include <cmath>
#include <iostream>

// Default constructor for Asteroid
Asteroid::Asteroid() : GameObject("Asteroid")
{
    mAngle = rand() % 360;
    mRotation = 0;
    mPosition.x = rand() % 800;
    mPosition.y = rand() % 600;
    mPosition.z = 0.0;

    float speedScale = 0.02f;
    mVelocity.x = speedScale * cos(DEG2RAD * mAngle);
    mVelocity.y = speedScale * sin(DEG2RAD * mAngle);
    mVelocity.z = 0.0;
}

// Destructor for Asteroid
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
    if (!mBoundingShape || !o->GetBoundingShape()) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
    for (auto& object : objects)
    {
        // Check if the colliding object is another asteroid
        if (object->GetType().GetTypeName() == "Asteroid" || object->GetType().GetTypeName() == "SmallAsteroid")
        {
            auto otherAsteroid = std::dynamic_pointer_cast<Asteroid>(object);
            if (otherAsteroid)
            {
                // Calculate the distance between the two asteroids
                GLVector3f deltaPosition = mPosition - otherAsteroid->mPosition;
                float distanceSquared = deltaPosition.x * deltaPosition.x + deltaPosition.y * deltaPosition.y;

                // Assume both asteroids have the same radius
                float radius = 10.0f; // Example radius, adjust as needed
                float combinedRadius = 2 * radius;

                // Debug output
                std::cout << "Asteroid 1 Position: (" << mPosition.x << ", " << mPosition.y << ")\n";
                std::cout << "Asteroid 2 Position: (" << otherAsteroid->mPosition.x << ", " << otherAsteroid->mPosition.y << ")\n";
                std::cout << "Distance Squared: " << distanceSquared << ", Combined Radius Squared: " << combinedRadius * combinedRadius << "\n";

                // Check if the asteroids are colliding
                if (distanceSquared <= combinedRadius * combinedRadius)
                {
                    std::cout << "Collision detected!\n";

                    // Calculate the relative velocity
                    GLVector3f deltaVelocity = mVelocity - otherAsteroid->mVelocity;

                    // Calculate the dot product of deltaVelocity and deltaPosition
                    float dotProduct = (deltaVelocity.x * deltaPosition.x + deltaVelocity.y * deltaPosition.y) / distanceSquared;

                    // Update the velocities based on the elastic collision formula
                    mVelocity.x -= dotProduct * deltaPosition.x;
                    mVelocity.y -= dotProduct * deltaPosition.y;

                    otherAsteroid->mVelocity.x += dotProduct * deltaPosition.x;
                    otherAsteroid->mVelocity.y += dotProduct * deltaPosition.y;
                }
            }
        }
		if (object->GetType().GetTypeName() == "Bullet")
		{
            int numSmallAsteroids = 1;

            for (int i = 0; i < numSmallAsteroids; i++)
            {
                // Create a new small asteroid
                shared_ptr<SmallAsteroid> smallAsteroid = make_shared<SmallAsteroid>();
                smallAsteroid->mVelocity = mVelocity * 1.5f;
                smallAsteroid->mScale = 0.1f;
                smallAsteroid->SetSprite(mSprite);
                shared_ptr<BoundingSphere> boundingSphere = make_shared<BoundingSphere>(smallAsteroid, 5.0f);
                smallAsteroid->SetBoundingShape(std::make_shared<BoundingSphere>(smallAsteroid->GetThisPtr(), 2.0f));
                mWorld->AddObject(smallAsteroid);
            }

            mWorld->FlagForRemoval(GetThisPtr());
		}
    }
}