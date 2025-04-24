#pragma once
#include <GL/glut.h>
#include <vector>
#include <string>
#include <memory>
#include "Asteroid.h"
class MainMenu
{
public:
	MainMenu();
	void Init();
	void Display();
	void Update();
	void HandleKeyboard(unsigned char key, int x, int y);
	void HandleSpecial(int key, int x, int y);

	bool ShouldStartGame() const;
	bool ShouldShowInstructions() const;
	bool ShouldShowHighScores() const;
	bool IsDifficultyEnabled() const;

	void SetGamerTag(const std::string& tag);

private:
	enum MenuItem {
		START_GAME,
		INSTRUCTIONS,
		HIGH_SCORES,
		QUIT,
		MENU_ITEM_COUNT
	};

	std::vector<std::shared_ptr<Asteroid>> mAsteroids;
	int mSelected;
	bool mStartGame;
	bool mShowInstructions;
	bool mShowHighScores;
	bool mDifficultyEnabled;
	std::string mGamerTag;

	void RenderText(float x, float y, const char* text);
};

