#include "GameWorld.h"
#include "Bullet.h"
#include "BoundingSphere.h"
#include "GameUtil.h"
#include "GameObject.h"
#include "Asteroid.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Bullets live for 2s by default. */
Bullet::Bullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int tt1)
	: GameObject("Bullet", p, v, a, h, r), mTimeToLive(tt1)
{
}

/** Copy constructor. */
Bullet::Bullet(const Bullet& b)
	: GameObject(b),
	  mTimeToLive(b.mTimeToLive)
{
}

/** Destructor. */
Bullet::~Bullet(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update bullet, removing it from game world if necessary. */
void Bullet::Update(int t)
{
	// Reduce time to live
	mTimeToLive -= t;
	// Ensure time to live isn't negative
	if (mTimeToLive < 0 && mWorld) { mTimeToLive = 0; }
	// If time to live is zero then remove bullet from world
	if (mTimeToLive == 0) {
		if (mWorld) mWorld->FlagForRemoval(GetThisPtr());
	}
	// Update position/velocity
	GameObject::Update(t);

}

bool Bullet::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Bullet::OnCollision(const GameObjectList& objects)
{
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		auto obj = *it;
		if (!obj || obj->GetType() != GameObjectType("Asteroid")) continue;

		float radius = 0.0f;
		auto shape = obj->GetBoundingShape();
		if (shape) {
			auto bs = dynamic_pointer_cast<BoundingSphere>(shape);
			if (bs) {
				radius = bs->GetRadius();
			}
		}

		if (radius > 4.0f && mWorld) {
			for (int i = 0; i < 2;++i) {
				GLVector3f pos = obj->GetPosition();
				GLVector3f vel(rand() % 6 - 3, rand() % 6 - 3, 0);
				shared_ptr<GameObject> smallAsteroid(new Asteroid(pos, vel));
				smallAsteroid->SetBoundingShape(make_shared<BoundingSphere>(smallAsteroid, 2.0f));
				mWorld->AddObject(smallAsteroid);
			}
		}

		if (mWorld) {
			mWorld->FlagForRemoval(obj);
		}
	}

	if (mWorld) {
		mWorld->FlagForRemoval(GetThisPtr());
	}
}
