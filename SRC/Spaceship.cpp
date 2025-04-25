#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "BoundingSphere.h"
#include "Asteroid.h"

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/**  Default constructor. */
Spaceship::Spaceship()
	: GameObject("Spaceship"), mThrust(0)
{
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. */
Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("Spaceship", p, v, a, h, r), mThrust(0)
{
}

/** Copy constructor. */
Spaceship::Spaceship(const Spaceship& s)
	: GameObject(s), mThrust(0)
{
}

/** Destructor. */
Spaceship::~Spaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update this spaceship. */
void Spaceship::Update(int t)
{
	// Call parent update function
	GameObject::Update(t);
}

/** Render this spaceship. */
void Spaceship::Render(void)
{
	if (mSpaceshipShape) mSpaceshipShape->Render();

	// If ship is thrusting
	if ((mThrust > 0) && (mThrusterShape)) mThrusterShape ->Render();

	GameObject::Render();
}

/** Fire the rockets. */
void Spaceship::Thrust(float t)
{
	mThrust = t;
	// Increase acceleration in the direction of ship
	mAcceleration.x = mThrust*cos(DEG2RAD*mAngle);
	mAcceleration.y = mThrust*sin(DEG2RAD*mAngle);
}

/** Set the rotation. */
void Spaceship::Rotate(float r)
{
	mRotation = r;
}

/** Shoot a bullet. */
void Spaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;
	// Construct a unit length vector in the direction the spaceship is headed
	GLVector3f spaceship_heading(cos(DEG2RAD*mAngle), sin(DEG2RAD*mAngle), 0);
	spaceship_heading.normalize();
	// Calculate the point at the node of the spaceship from position and heading
	GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
	// Calculate how fast the bullet should travel
	float bullet_speed = 30;
	// Construct a vector for the bullet's velocity
	GLVector3f bullet_velocity = mVelocity + spaceship_heading * bullet_speed;
	// Construct a new bullet
	shared_ptr<GameObject> bullet
		(new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(bullet);

}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Spaceship::OnCollision(const GameObjectList& objects)
{
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		shared_ptr<GameObject> obj = *it;
		if (!obj) continue;

		if (auto asteroid = dynamic_pointer_cast<Asteroid>(obj)) {
			float radius = 0.0f;
			auto shape = obj->GetBoundingShape();
			if (shape) {
				auto bs = dynamic_pointer_cast<BoundingSphere>(shape);
				if (bs) {
					radius = bs->GetRadius();
				}

				if (radius <= 4.0f) {
					// Bounce
					GLVector3f v1 = mVelocity;
					GLVector3f v2 = obj->GetVelocity();

					mVelocity = v2 * 0.5f;
					obj->SetVelocity(v1 * 0.5f);
				}
				else {
					if (mWorld) {
						mWorld->FlagForRemoval(GetThisPtr());
						mWorld->FlagForRemoval(obj);
					}
				}
			}
		}
	}
}