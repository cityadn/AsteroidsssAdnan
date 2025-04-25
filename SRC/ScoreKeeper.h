#ifndef __SCOREKEEPER_H__
#define __SCOREKEEPER_H__

#include "GameUtil.h"

#include "GameObject.h"
#include "GameObjectType.h"
#include "IScoreListener.h"
#include "IGameWorldListener.h"
#include "BoundingSphere.h"

class ScoreKeeper : public IGameWorldListener
{
public:
	ScoreKeeper() { mScore = 0; }
	virtual ~ScoreKeeper() {}

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}

	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
	{
		if (object->GetType() == GameObjectType("Asteroid")) {
			float radius = 0.0f;
			auto shape = object->GetBoundingShape();
			if (shape) {
				auto bs = dynamic_pointer_cast<BoundingSphere>(shape);
				if (bs) {
					radius = bs->GetRadius();
				}
			}

			if (radius > 4.0f) {
				// Add score
				mScore += 20;
			}
			else {
				// Add score
				mScore += 10;
			}

			FireScoreChanged();
		}
	}

	void AddListener(shared_ptr<IScoreListener> listener)
	{
		mListeners.push_back(listener);
	}

	void FireScoreChanged()
	{
		// Send message to all listeners
		for (auto lit = mListeners.begin(); lit != mListeners.end(); ++lit) {
			(*lit)->OnScoreChanged(mScore);
		}
	}

private:
	int mScore;

	typedef std::list< shared_ptr<IScoreListener> > ScoreListenerList;

	ScoreListenerList mListeners;
};

#endif
