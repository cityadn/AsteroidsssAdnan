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
    //if (GetType() == o->GetType()) return false;
    if (!mBoundingShape || !o->GetBoundingShape()) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
    for (auto& object : objects)
    {
        // Check if the colliding object is another asteroid
        if (object->GetType().GetTypeName() == "Asteroid")
        {
            auto otherAsteroid = std::dynamic_pointer_cast<Asteroid>(object);
            if (otherAsteroid)
            {
                // Calculate the distance between the two asteroids
                GLVector3f deltaPosition = mPosition - otherAsteroid->mPosition;
                float distanceSquared = deltaPosition.x * deltaPosition.x + deltaPosition.y * deltaPosition.y;

                // Assume both asteroids have the same radius
                float radius = 20.0f; // Example radius, adjust as needed
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
    }
}