#include <windows.h>	// The standard header for Windows applications
#include <time.h>		// The standard header to access time fuctions

#include "GlutSession.h"
#include "Asteroids.h"
#include "MainMenu.h"
#include <GL/glut.h>
#include <string>
#include <ctime>


// Now we need to perform some Windows magic to stop an extra console
// window from appearing, don't worry about the specifics of this.
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

MainMenu* menu = nullptr;
Asteroids* asteroids = nullptr;
bool isGameStarted = false;
std::string playerGameTag = "";

enum AppState {
	STATE_MENU,
	STATE_GAME,
	STATE_GAME_OVER
};

AppState currentState = STATE_MENU;

void DisplayCallback() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentState == STATE_MENU) {
		menu->Display();
	}
	else if (currentState == STATE_GAME) {
		asteroids->Render();
	}
	else if (currentState == STATE_GAME_OVER) {
		// Display game over screen and ask for gamer tag
		
	}

	glutSwapBuffers();
}

// GLUT reshape function
void ReshapeCallback(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

// GLUT idle function
void IdleCallback() {
	if (currentState == STATE_MENU) {
		menu->Update();
	}
	else if (currentState == STATE_GAME) {
		asteroids->Update();
	}
	glutPostRedisplay(); // keeps screen updating
}

// Called when the menu option "Start Game" is selected
void StartGame() {
	asteroids->Start();
	currentState = STATE_GAME;
}

void OnGamerTagEntered(const std::string& tag) {
	playerGameTag = tag;
	// Save the gamer tag to the high scores table
	// You may want to save this to a file or memory for high-score management.
}

void RenderText(float x, float y, const char* text) {
	glRasterPos2f(x, y);
	while (*text)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
	}
}

void HandleKeyboard(unsigned char key, int x, int y) {
	if (currentState == STATE_MENU) {
		menu->HandleKeyboard(key, x, y);
		if (menu->ShouldStartGame()) {
			StartGame();
			isGameStarted = true;
		}
	}
	else if (currentState == STATE_GAME_OVER) {
		if (key == 13) { // Enter key pressed, accept the gamer tag
			OnGamerTagEntered(playerGameTag);
			currentState = STATE_MENU; // Go back to menu or restart the game
		}
	}
}

void HandleSpecial(int key, int x, int y) {
	if (currentState == STATE_MENU) {
		menu->HandleSpecial(key, x, y);
	}
}

// Main Function For Bringing It All Together.
int main(int argc, char* argv[])
{
	// Initialise random number generator
	srand((unsigned)time(NULL));
	// Initialise a unique GLUT session
	GlutSession::GetInstance().Init(argc, argv);

	menu = new MainMenu();
	asteroids = new Asteroids(argc, argv);

	currentState = STATE_MENU;

	glutDisplayFunc(DisplayCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(HandleKeyboard);
	glutSpecialFunc(HandleSpecial);

	glutMainLoop();
	// Create a new asteroids game
	//Asteroids asteroids(argc, argv);
	// Start the asteroids game
	//asteroids.Start();

	return 0;
}
