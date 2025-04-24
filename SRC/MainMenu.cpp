#include "MainMenu.h"
#include <GL/glut.h>
#include <ctime>

const char* menuLabels[] = {
	"Start Game",
	"Instructions",
	"High Scores",
	"Quit"
};

MainMenu::MainMenu() : mSelected(0), mStartGame(false)
{
	srand((unsigned)time(nullptr));
	for (int i = 0; i < 10; ++i) {
		auto asteroid = std::make_shared<Asteroid>();
		asteroid->SetPosition(GLVector3f(rand() % 100 - 50, rand() % 100 - 50, 0));
		asteroid->SetVelocity(GLVector3f((rand() % 200 - 100) / 100.0f, (rand() % 200 - 100) / 100.0f, 0));
		mAsteroids.push_back(asteroid);
	}
}

void MainMenu::Update() {
	for (auto& asteroid : mAsteroids) {
		asteroid->Update(16);
	}
}

void MainMenu::Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& asteroid : mAsteroids) {
		asteroid->Render();
	}

	for (int i = 0; i < MENU_ITEM_COUNT; ++i) {
		glColor3f(i == mSelected ? 1.0f : 0.6f, 1.0f, 1.0f);
		RenderText(-0.1f, 0.3f - i * 0.1f, menuLabels[i]);
	}

	glutSwapBuffers();
}

void MainMenu::RenderText(float x, float y, const char* text) {
	glRasterPos2f(x, y);
	while (*text)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
	}
}

void MainMenu::HandleKeyboard(unsigned char key, int, int) {
	if (key == 13) { // Enter 
		if (mSelected == START_GAME) {
			mStartGame = true;
		}
		else if (mSelected == QUIT) {
			exit(0);
		}
	}
	else if (key == 27) { // Escape to exit
		exit(0);
	}
}

void MainMenu::HandleSpecial(int key, int, int) {
	if (key == GLUT_KEY_UP) {
		mSelected = (mSelected - 1 + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
	}
	else if (key == GLUT_KEY_DOWN) {
		mSelected = (mSelected + 1) % MENU_ITEM_COUNT;
	}
}

bool MainMenu::ShouldStartGame() const {
	return mStartGame;
}
