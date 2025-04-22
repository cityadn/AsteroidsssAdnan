#include "MainMenu.h"

MainMenu:MainMenu(float width, float height)
{
	if (!font.loadFromFile("AlexandriaFLF.ttf"))
	{
		cout << "No font is here";
	}
}