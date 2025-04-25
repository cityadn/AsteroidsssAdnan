#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"

using namespace std;

Asteroid::Asteroid(void) : GameObject("Asteroid"), mSize("Large")
{
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD*mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD*mAngle);
	mVelocity.z = 0.0;
}

Asteroid::Asteroid(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, std::string size)
	: GameObject("Asteroid", p, v, a, h, r), mSize(size)
{

}

Asteroid::Asteroid(const Asteroids& a) : GameObject(a), mSize(a.mSize)
{
	mAngle = a.mAngle;
	mRotation = a.mRotation;
	mPosition = a.mPosition;
	mVelocity = a.mVelocity;
}

Asteroid::~Asteroid(void)
{
}

void Asteroid::Update(int t) {
	GameObject::Update(t);
}

bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
	for (const auto& obj : objects) {
		if (obj->GetType() == GameObjectType("Asteroid")) {
			shared_ptr<Asteroid> a = std::static_pointer_cast<Asteroid>(obj);
			Bounce(other);
		}

		if (obj->GetType() == GameObjectType("Spaceship")) {
			shared_ptr<Spaceship> s = static_pointer_cast<Spaceship>(obj);
			mWorld->FlagForRemoval(GetThisPtr());
			mWorld->FlagForRemoval(spaceship);
			spaceship->OnCollision(objects);
		}

		if (obj->GetType() == GameObjectType("Bullet")) {
			shared_ptr<Bullet> bullet = static_pointer_cast<Bullet>(obj);
			shared_ptr<Asteroid> smallAsteroid1 = CreateSmallerAsteroid();
			shared_ptr<Asteroid> smallAsteroid2 = CreateSmallerAsteroid();
			mWorld->AddObject(smallAsteroid1);
			mWorld->AddObject(smallAsteroid2);

			mWorld->FlagForRemoval(GetThisPtr());
			mWorld->FlagForRemoval(bullet);

			// Award score based on asteroid size
			int score = (mSize == "Large") ? 100 : 50;
			mWorld->GetScoreKeeper()->AddScore(score);
		}

	}
}

void Asteroid::Bounce(shared_ptr<Asteroid> other)
{
	GLVector3f direction = mPosition - other->mPosition;
	direction.normalize();

	// Calculate relative velcocity (velocity difference)
	GLVector3f relativeVelocity = mVelocity - other->mVelocity;

	// Dot product of relative velocity and direction
	float speed = relativeVeclocity.dot(direction);

	// Bounce effect - reverse velocities based on collision angle
	if (speed > 0) {
		// Reflect velocities
		mVelocity -= 2 * speed * direction;
		other->mVelocity += 2 * speed * direction;
	}
}

/** Create a smaller asteroid. */
shared_ptr<Asteroid> Asteroid::CreateSmallerAsteroid()
{
	GLVector3f randomVelocity(rand() % 10 - 5, rand() % 10 - 5, 0); // Random small velocity
	return make_shared<Asteroid>(mPosition, randomVelocity, GLVector3f(), 0, 0, "Small");
}
