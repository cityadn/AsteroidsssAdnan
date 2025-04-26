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
#include <cmath> // For sqrt

// Define a maximum limit for asteroids
const int MAX_ASTEROIDS = 50;
const int MAX_INITIAL_ASTEROIDS = 10; // Increased number of asteroids created initially
const float ASTEROID_SPEED_SCALE = 0.02f; // Lowered asteroid speed

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
    CreateAsteroids(MAX_INITIAL_ASTEROIDS);

    // Create the GUI
    CreateGUI();

    // Add a player (watcher) to the game world
    mGameWorld->AddListener(&mPlayer);

    // Add this class as a listener of the player
    mPlayer.AddListener(thisPtr);

    // Start the game
    GameSession::Start();
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

    if (value == SHOW_GAME_OVER)
    {
        mGameOverLabel->SetVisible(true);
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

/** Handle object removal. */
void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
    if (object->GetType() == GameObjectType("Asteroid"))
    {
        shared_ptr<GameObject> explosion = CreateExplosion();
        explosion->SetPosition(object->GetPosition());
        explosion->SetRotation(object->GetRotation());
        mGameWorld->AddObject(explosion);
        mAsteroidCount--;
    }
}

// PRIVATE INSTANCE METHODS ///////////////////////////////////////////////////

/** Create asteroids. */
void Asteroids::CreateAsteroids(const uint num_asteroids)
{
    uint asteroids_to_create = std::min(num_asteroids, MAX_ASTEROIDS - mAsteroidCount);
    for (uint i = 0; i < asteroids_to_create; i++)
    {
        Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
        shared_ptr<Sprite> asteroid_sprite =
            make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
        asteroid_sprite->SetLoopAnimation(true);

        shared_ptr<Asteroid> asteroid = make_shared<Asteroid>();
        asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
        asteroid->SetSprite(asteroid_sprite);
        asteroid->SetScale(0.2f);

        // Adjust asteroid velocity to make it slower
        GLVector3f velocity = asteroid->GetVelocity();
        velocity.x *= ASTEROID_SPEED_SCALE;
        velocity.y *= ASTEROID_SPEED_SCALE;
        asteroid->SetVelocity(velocity);

        mGameWorld->AddObject(asteroid);
        mAsteroidCount++;
    }
}

/** Handle asteroid collision to bounce in the opposite direction. */
void Asteroids::HandleAsteroidCollision(shared_ptr<GameObject> asteroid1, shared_ptr<GameObject> asteroid2)
{
    // Get positions and velocities of the asteroids
    GLVector3f position1 = asteroid1->GetPosition();
    GLVector3f position2 = asteroid2->GetPosition();
    GLVector3f velocity1 = asteroid1->GetVelocity();
    GLVector3f velocity2 = asteroid2->GetVelocity();

    // Calculate the collision normal
    GLVector3f collision_normal = position2 - position1;
    float magnitude = std::sqrt(collision_normal.x * collision_normal.x +
        collision_normal.y * collision_normal.y +
        collision_normal.z * collision_normal.z);
    if (magnitude > 0.0f) {
        collision_normal.x /= magnitude;
        collision_normal.y /= magnitude;
        collision_normal.z /= magnitude;
    }
    else {
        // If the magnitude is zero, the asteroids are at the same position
        return;
    }

    // Calculate dot products
    float dot_product1 = velocity1.x * collision_normal.x +
        velocity1.y * collision_normal.y +
        velocity1.z * collision_normal.z;
    float dot_product2 = velocity2.x * collision_normal.x +
        velocity2.y * collision_normal.y +
        velocity2.z * collision_normal.z;

    // Reflect velocities along the collision normal
    GLVector3f new_velocity1 = {
        velocity1.x - 2 * dot_product1 * collision_normal.x,
        velocity1.y - 2 * dot_product1 * collision_normal.y,
        velocity1.z - 2 * dot_product1 * collision_normal.z
    };

    GLVector3f new_velocity2 = {
        velocity2.x - 2 * dot_product2 * collision_normal.x,
        velocity2.y - 2 * dot_product2 * collision_normal.y,
        velocity2.z - 2 * dot_product2 * collision_normal.z
    };

    // Apply the new velocities
    asteroid1->SetVelocity(new_velocity1);
    asteroid2->SetVelocity(new_velocity2);

    // Debug output to verify the new velocities
    std::cout << "Asteroid 1 new velocity: (" << new_velocity1.x << ", " << new_velocity1.y << ", " << new_velocity1.z << ")" << std::endl;
    std::cout << "Asteroid 2 new velocity: (" << new_velocity2.x << ", " << new_velocity2.y << ", " << new_velocity2.z << ")" << std::endl;
}

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