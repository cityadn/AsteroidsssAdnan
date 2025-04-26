#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h"
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener, public ITimerListener
{
public:
    Asteroids(int argc, char* argv[]);
    virtual ~Asteroids(void);

    virtual void Start(void);
    virtual void Stop(void);

    // Declaration of IKeyboardListener interface ////////////////////////////////
    void OnKeyPressed(uchar key, int x, int y) override;
    void OnKeyReleased(uchar key, int x, int y) override;
    void OnSpecialKeyPressed(int key, int x, int y) override;
    void OnSpecialKeyReleased(int key, int x, int y) override;

    // Declaration of IScoreListener interface //////////////////////////////////
    void OnScoreChanged(int score) override;

    // Declaration of the IPlayerListener interface /////////////////////////////
    void OnPlayerKilled(int lives_left) override;

    // Declaration of IGameWorldListener interface //////////////////////////////
    void OnWorldUpdated(GameWorld* world) override {}
    void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) override {}
    void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object) override;

    // Override the default implementation of ITimerListener ////////////////////
    void OnTimer(int value) override;

private:
    shared_ptr<Spaceship> mSpaceship;
    shared_ptr<GUILabel> mScoreLabel;
    shared_ptr<GUILabel> mLivesLabel;
    shared_ptr<GUILabel> mGameOverLabel;

    uint mLevel;
    uint mAsteroidCount;

    void ResetSpaceship();
    shared_ptr<GameObject> CreateSpaceship();
    void CreateGUI();
    void CreateAsteroids(const uint num_asteroids);
    shared_ptr<GameObject> CreateExplosion();

    // New private methods and variables for gradual asteroid spawning
    void SpawnAsteroid(); // Spawns a single asteroid
    int mTimeSinceLastSpawn; // Time since the last asteroid spawn
    const int SPAWN_INTERVAL = 2000; // Spawn interval in milliseconds

    const static uint SHOW_GAME_OVER = 0;
    const static uint START_NEXT_LEVEL = 1;
    const static uint CREATE_NEW_PLAYER = 2;

    ScoreKeeper mScoreKeeper;
    Player mPlayer;
};

#endif