#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "GameObjectType.h"
#include "GameWorld.h"

#include <GL/GLUT.H>
#include <cmath>
#include <cstdlib>

using namespace std;

Asteroid::Asteroid(void)
	: GameObject("Asteroid"),
	mSize("Large"),
	mBoundingCircle(GLVector3f(0, 0, 0), 5.0f)
{
	mRadius = 10.0f;

	// Randomize initial position and velocity
	GLVector3f pos = GLVector3f(rand() % 200 - 100, rand() % 200 - 100, 0);
	GLVector3f vel = GLVector3f(rand() % 40 - 20, rand() % 40 - 20, 0);
	SetPosition(pos);
	SetVelocity(vel);
}

Asteroid::Asteroid(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, std::string size)
	: GameObject("Asteroid", p, v, a, h, r),
	mSize(size),
	mBoundingCircle(p, 5.0f),
	mRadius(r)
{

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
	if (o->GetType() == GameObjectType("Bullet") || o->GetType() == GameObjectType("Spaceship"))
	{
		GLVector3f thisPos = GetPosition();
		GLVector2f otherPos = o->GetPosition();

		float otherRadius = 4.0f;
		float dx = thisPos.x - otherPos.x;
		float dy = thisPos.y - otherPos.y;
		float dist = sqrt(dx * dx + dy * dy);

		return dist < (mBoundingCircle.radius + otherRadius);
	}
	return false;
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		auto obj = *it;
		if (!obj) continue;
		if (obj->GetType() == GameObjectType("Asteroid")) {
			auto a = static_pointer_cast<Asteroid>(obj);
			Bounce(a);
		}
		if (obj->GetType() == GameObjectType("Spaceship")) {
			auto s = static_pointer_cast<Spaceship>(obj);
			mWorld->FlagForRemoval(GetThisPtr());
			mWorld->FlagForRemoval(s);
			s->OnCollision(objects);
		}
		if (obj->GetType() == GameObjectType("Bullet")) {
			auto bullet = static_pointer_cast<Bullet>(obj);
			auto smallAsteroid1 = CreateSmallerAsteroid();
			auto smallAsteroid2 = CreateSmallerAsteroid();
			if (smallAsteroid1) mWorld->AddObject(smallAsteroid1);
			if (smallAsteroid2) mWorld->AddObject(smallAsteroid2);
			mWorld->FlagForRemoval(GetThisPtr());
			mWorld->FlagForRemoval(bullet);
			// Award score based on asteroid size
			int score = (mSize == "Large") ? 100 : 50;
		}
	}
}

void Asteroid::Bounce(shared_ptr<Asteroid> other)
{
	GLVector3f normal = (GetPosition() - other->GetPosition()).unit();
	GLVector3f v1 = GetVelocity();
	GLVector3f v2 = other->GetVelocity();

	GLVector3f new_v1 = v1 - normal * (v1 - v2).dot(normal);
	GLVector3f new_v2 = v2 - normal * (v2 - v1).dot(normal);

	SetVelocity(new_v1);
	other->SetVelocity(new_v2);
}

/** Create a smaller asteroid. */
shared_ptr<Asteroid> Asteroid::CreateSmallerAsteroid()
{
	GLVector3f newVel = GLVector3f(rand() % 40 - 20, rand() % 40 - 20, 0);
	GLfloat newRadius = mRadius / 2.0f;

	std::string newSize;
	if (mSize == "Large") {
		newSize = "Medium";
	}
	else if (mSize == "Medium") {
		newSize = "Small";
	}
	else {
		return nullptr;
	}

	std::shared_ptr<Asteroid> smaller = std::make_shared<Asteroid>(GetPosition(), newVel, GLVector3f(0, 0, 0), 0, newRadius, newSize);

	smaller->SetBoundingShape(make_shared<BoundingSphere>(smaller, newRadius));

	return smaller;
}
