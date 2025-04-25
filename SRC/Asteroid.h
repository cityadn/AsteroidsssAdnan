#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"
#include "GameUtil.h"
#include "BoundingShape.h"
#include "Spaceship.h"
#include "Bullet.h"

class Asteroid : public GameObject, public std::enable_shared_from_this<Asteroid>
{
public:
	Asteroid();
	Asteroid(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, std::string size);
	Asteroid(const Asteroid& a);
	Asteroid(GLVector3f pos, GLVector3f vel) 
		: GameObject("Asteroid", pos, vel, GLVector3f(0, 0, 0), 0, 0), mSize("Large")
	{
		mBoundingCircle = BoundingCircle(pos, 5.0f);
	}
	virtual ~Asteroid(void);

	virtual void Update(int t) override;
	virtual void Render() override;
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

	void Bounce(shared_ptr<Asteroid> other);
	shared_ptr<Asteroid> CreateSmallerAsteroid();
	std::string GetSize() { return mSize; }
	std::shared_ptr<BoundingShape> mAsteroidShape;
	
private:
	std::string mSize;
	struct BoundingCircle {
		GLVector3f center;
		float radius;

		BoundingCircle(GLVector3f p = GLVector3f(0, 0, 0), float r = 5.0f) : center(p), radius(r) {}

		bool CollidesWith(const BoundingCircle& other) {
			float distance = (center - other.center).length();
			return distance <= (radius + other.radius);
		}
	};

	BoundingCircle mBoundingCircle;
	float mRadius;
};

#endif
