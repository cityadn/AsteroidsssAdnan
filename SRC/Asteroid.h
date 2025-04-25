#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"
#include "GameUtil.h"
#include "BoundingShape.h"
#include "Spaceship.h"
#include "Bullet.h"

class Asteroid : public GameObject
{
public:
	Asteroid();
	Asteroid(GLVector3f p, GLVector3f v, GLfloat h, GLfloat r, std::string size);
	Asteroid(const Asteroid& a);
	virtual ~Asteroid(void);

	virtual void Update(int t);
	virtual void OnCollision(const GameObjectList& objects);
	virtual void Rendr(void);

	void Bounce(shared_ptr<Asteroid> other);
	shared_ptr<Asteroid> CreateSmallerAsteroid();

	std::string GetSize() { return mSize; }
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

private:
	std::string mSize;
};

#endif
