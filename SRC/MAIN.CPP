#include <windows.h>	// The standard header for Windows applications
#include <time.h>		// The standard header to access time fuctions

#include "GlutSession.h"
#include "Asteroids.h"
#include "MainMenu.h"
#include <GL/glut.h>


// Now we need to perform some Windows magic to stop an extra console
// window from appearing, don't worry about the specifics of this.
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// Main Function For Bringing It All Together.
int main(int argc, char* argv[])
{
	// Initialise random number generator
	srand((unsigned)time(NULL));
	// Initialise a unique GLUT session
	GlutSession::GetInstance().Init(argc, argv);
	// Create a new asteroids game
	//Asteroids asteroids(argc, argv);
	std::shared_ptr<Asteroids> asteroids = std::make_shared<Asteroids>(argc, argv);
	// Start the asteroids game
	asteroids->Start();

	return 0;
}
