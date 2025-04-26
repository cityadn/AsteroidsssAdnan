#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include <vector>
#include <memory>
#include <sstream>

// Define a maximum limit for asteroids
const int MAX_ASTEROIDS = 50;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char* argv[])
    : GameSession(argc, argv), mTimeSinceLastSpawn(0)
{
    mLevel = 0;
    mAsteroidCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
    // Create a shared pointer for the Asteroids game object - DO NOT REMOVE
    shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

    // Add this class as a listener of the game world
    mGameWorld->AddListener(thisPtr.get());

    // Add this as a listener to the world and the keyboard
    mGameWindow->AddKeyboardListener(thisPtr);

    // Add a score keeper to the game world
    mGameWorld->AddListener(&mScoreKeeper);

    // Add this class as a listener of the score keeper
    mScoreKeeper.AddListener(thisPtr);

    // Create an ambient light to show sprite textures
    GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glEnable(GL_LIGHT0);

    AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
    AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
    AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

    // Create a spaceship and add it to the world
    mGameWorld->AddObject(CreateSpaceship());
    // Create some asteroids and add them to the world
    CreateAsteroids(10);

    // Create the GUI
    CreateGUI();

    // Add a player (watcher) to the game world
    mGameWorld->AddListener(&mPlayer);

    // Add this class as a listener of the player
    mPlayer.AddListener(thisPtr);

    // Start the game
    GameSession::Start();

    // Set a timer for asteroid spawning
    SetTimer(SPAWN_INTERVAL, 0);
}

/** Stop the current game. */
void Asteroids::Stop()
{
    // Stop the game
    GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

/** Handle timer events. */
void Asteroids::OnTimer(int value)
{
    if (value == CREATE_NEW_PLAYER)
    {
        mSpaceship->Reset();
        mGameWorld->AddObject(mSpaceship);
    }

    if (value == START_NEXT_LEVEL)
    {
        mLevel++;
        int num_asteroids = 10 + 2 * mLevel;
        CreateAsteroids(num_asteroids);
    }

    if (value == SHOW_GAME_OVER)
    {
        mGameOverLabel->SetVisible(true);
    }

    // Gradual asteroid spawning
    if (mAsteroidCount < MAX_ASTEROIDS) // Check if the maximum limit is reached
    {
        mTimeSinceLastSpawn += SPAWN_INTERVAL;
        if (mTimeSinceLastSpawn >= SPAWN_INTERVAL)
        {
            SpawnAsteroid();
            mTimeSinceLastSpawn = 0;
        }

        // Continue the timer
        SetTimer(SPAWN_INTERVAL, 0);
    }
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
    switch (key)
    {
    case ' ':
        mSpaceship->Shoot();
        break;
    default:
        break;
    }
}

void Asteroids::OnKeyReleased(uchar key, int x, int y)
{
    // Handle key release events if needed
}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        mSpaceship->Thrust(10);
        break;
    case GLUT_KEY_LEFT:
        mSpaceship->Rotate(90);
        break;
    case GLUT_KEY_RIGHT:
        mSpaceship->Rotate(-90);
        break;
    default:
        break;
    }
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        mSpaceship->Thrust(0);
        break;
    case GLUT_KEY_LEFT:
        mSpaceship->Rotate(0);
        break;
    case GLUT_KEY_RIGHT:
        mSpaceship->Rotate(0);
        break;
    default:
        break;
    }
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IScoreListener ////////////////////////

void Asteroids::OnScoreChanged(int score)
{
    std::ostringstream msg_stream;
    msg_stream << "Score: " << score;
    mScoreLabel->SetText(msg_stream.str());
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IPlayerListener ///////////////////////

void Asteroids::OnPlayerKilled(int lives_left)
{
    shared_ptr<GameObject> explosion = CreateExplosion();
    explosion->SetPosition(mSpaceship->GetPosition());
    explosion->SetRotation(mSpaceship->GetRotation());
    mGameWorld->AddObject(explosion);

    std::ostringstream msg_stream;
    msg_stream << "Lives: " << lives_left;
    mLivesLabel->SetText(msg_stream.str());

    if (lives_left > 0)
    {
        SetTimer(1000, CREATE_NEW_PLAYER);
    }
    else
    {
        SetTimer(500, SHOW_GAME_OVER);
    }
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
    if (object->GetType() == GameObjectType("Asteroid"))
    {
        shared_ptr<GameObject> explosion = CreateExplosion();
        explosion->SetPosition(object->GetPosition());
        explosion->SetRotation(object->GetRotation());
        mGameWorld->AddObject(explosion);
        mAsteroidCount--;

        if (mAsteroidCount <= 0)
        {
            SetTimer(500, START_NEXT_LEVEL);
        }
    }
}

// PRIVATE INSTANCE METHODS ///////////////////////////////////////////////////

/** Create a spaceship. */
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
    mSpaceship = make_shared<Spaceship>();
    mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
    shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
    mSpaceship->SetBulletShape(bullet_shape);
    Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
    shared_ptr<Sprite> spaceship_sprite =
        make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
    mSpaceship->SetSprite(spaceship_sprite);
    mSpaceship->SetScale(0.1f);
    mSpaceship->Reset();
    return mSpaceship;
}

/** Create the GUI. */
void Asteroids::CreateGUI()
{
    mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));

    mScoreLabel = make_shared<GUILabel>("Score: 0");
    mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
    shared_ptr<GUIComponent> score_component =
        static_pointer_cast<GUIComponent>(mScoreLabel);
    mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

    mLivesLabel = make_shared<GUILabel>("Lives: 3");
    mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
    shared_ptr<GUIComponent> lives_component =
        static_pointer_cast<GUIComponent>(mLivesLabel);
    mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

    mGameOverLabel = make_shared<GUILabel>("GAME OVER");
    mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
    mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
    mGameOverLabel->SetVisible(false);
    shared_ptr<GUIComponent> game_over_component =
        static_pointer_cast<GUIComponent>(mGameOverLabel);
    mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));
}

/** Create asteroids. */
void Asteroids::CreateAsteroids(const uint num_asteroids)
{
    mAsteroidCount = num_asteroids;
    for (uint i = 0; i < num_asteroids; i++)
    {
        Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
        shared_ptr<Sprite> asteroid_sprite =
            make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
        asteroid_sprite->SetLoopAnimation(true);
        shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
        asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
        asteroid->SetSprite(asteroid_sprite);
        asteroid->SetScale(0.2f);
        mGameWorld->AddObject(asteroid);
    }
}

/** Create an explosion. */
shared_ptr<GameObject> Asteroids::CreateExplosion()
{
    Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
    shared_ptr<Sprite> explosion_sprite =
        make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
    explosion_sprite->SetLoopAnimation(false);
    shared_ptr<GameObject> explosion = make_shared<Explosion>();
    explosion->SetSprite(explosion_sprite);
    explosion->Reset();
    return explosion;
}

/** Spawn a single asteroid. */
void Asteroids::SpawnAsteroid()
{
    if (mAsteroidCount < MAX_ASTEROIDS) // Limit the total number of asteroids
    {
        Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
        shared_ptr<Sprite> asteroid_sprite =
            make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
        asteroid_sprite->SetLoopAnimation(true);
        shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
        asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
        asteroid->SetSprite(asteroid_sprite);
        asteroid->SetScale(0.2f);
        mGameWorld->AddObject(asteroid);
        mAsteroidCount++;
    }
}