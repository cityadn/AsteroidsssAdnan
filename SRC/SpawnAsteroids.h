#pragma once
#include "Asteroid.h"
#include "GameWorld.h"
#include <memory>
#include <stdexcept>
#include "Asteroids.h"

// Function to spawn asteroids
void SpawnAsteroids(GameWorld* world)
{
    try
    {
        auto asteroid = std::make_shared<Asteroid>();
        world->AddObject(asteroid);
    }
    catch (const std::runtime_error& e)
    {
        // Maximum number of asteroids reached, do nothing
    }
}

// Call this function periodically in your game loop
void UpdateAsteroidSpawning(GameWorld* world, int elapsedTime)
{
    static int timeSinceLastSpawn = 0;
    const int SPAWN_INTERVAL = 2000; // Spawn every 2000ms (2 seconds)

    timeSinceLastSpawn += elapsedTime;

    if (timeSinceLastSpawn >= SPAWN_INTERVAL)
    {
        SpawnAsteroids(world);
        timeSinceLastSpawn = 0;
    }
}