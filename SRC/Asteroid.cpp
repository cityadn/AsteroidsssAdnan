#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"
#include "Spaceship.h"
#include "Bullet.h"

using namespace std;

Asteroid::Asteroid()
	: GameObject("Asteroid", GLVector3f(0, 0, 0), GLVector3f(0, 0, 0), GLVector3f(0, 0, 0), 0, 0), mSize("Large")
{
	mBoundingCircle = BoundingCircle(GLVector3f(0, 0, 0), 5.0f);
}

Asteroid::Asteroid(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, std::string size)
	: GameObject("Asteroid", p, v, a, h, r), mSize(size), mBoundingCircle(p, 5.0f)
{
	//mBoundingShape = make_shared<BoundingShape>(this, 2.0f);
	mBoundingCircle = BoundingCircle(p, r);
}

Asteroid::~Asteroid(void)
{
}

void Asteroid::Update(int t) {
	GameObject::Update(t);
	mBoundingCircle.center = GetPosition();
}

void Asteroid::Render()
{
	glPushMatrix();

	glTranslatef(mPosition.x, mPosition.y, mPosition.z);

	glColor3f(1.0f, 1.0f, 1.0f);
	glutSolidSphere(mBoundingCircle.radius, 10, 10);
	GameObject::Render();

	glBegin(GL_POLYGON);
	float radius = 2.0f;
	for (int i = 0; i < 360; i += 10) {
		float angle = 3.14159f / 180.0f;
		float x = radius * cos(angle * i);
		float y = radius * sin(angle * i);
		glVertex2f(x, y);
	}
	glEnd();

	glPopMatrix();
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
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		auto obj = *it;
		if (!obj) continue;
		if (obj->GetType() == GameObjectType("Asteroid")) {
			shared_ptr<Asteroid> a = std::static_pointer_cast<Asteroid>(obj);
			Bounce(a);
		}
		if (obj->GetType() == GameObjectType("Spaceship")) {
			shared_ptr<Spaceship> s = static_pointer_cast<Spaceship>(obj);
			mWorld->FlagForRemoval(GetThisPtr());
			mWorld->FlagForRemoval(s);
			s->OnCollision(objects);
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
	float speed = relativeVelocity.dot(direction);

	// Bounce effect - reverse velocities based on collision angle
	if (speed > 0) {
		// Reflect velocities
		GLVector3f impulse = direction;
		impulse.x *= 2 * speed;
		impulse.y *= 2 * speed;
		impulse.z *= 2 * speed;

		mVelocity -= impulse;
		other->mVelocity += impulse;
	}
}

/** Create a smaller asteroid. */
shared_ptr<Asteroid> Asteroid::CreateSmallerAsteroid()
{
	GLVector3f randVel(rand() % 10 - 5, rand() % 10 - 5, 0); // Random small velocity
	return make_shared<Asteroid>(mPosition, randVel, GLVector3f(), 0, 0, "Small");
}
