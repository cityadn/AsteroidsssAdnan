// Computer Science and Games Technology: Lab Exercise 01

// First we need to include some header files...
#include <windows.h>	// The standard header for Windows applications
#include <time.h>		// The standard header to access time fuctions

#include "GlutSession.h"
#include "Asteroids.h"

#include "sfml.h"
#include <SFML/Graphics.hpp>
#include "MainMenu.h"
using namespace sf;

// Now we need to perform some Windows magic to stop an extra console
// window from appearing, don't worry about the specifics of this.
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// Main Function For Bringing It All Together.
int main(int argc, char* argv[])
{
	// Make a Main Window
	RenderWindow MENU(VideoMode(960, 720), "Main Menu", Style::Default);
	MainMenu mainMenu(MENU.getSize().x, MENU.getSize().y);

	while (MENU.isOpen())
	{
		Event event;
		while (MENU.pollEvent(event))
		{
			if (event.type == Event::Closed)
				MENU.close();
			if (event.type == Event::KeyReleased)
			{
				if (event.key.code == Keyboard::Up)
					mainMenu.MoveUp();
				break;
				if (event.key.code == Keyboard::Down)
					mainMenu.MoveDown();
				break;
				if (event.key.code == Keyboard::Return)
				{
					RenderWindow Play(VideoMode(960, 720), "Asteroidsss", Style::Default);
					RenderWindow OPTIONS(VideoMode(960, 720), "OPTIONS", Style::Default);
					RenderWindow ABOUT(VideoMode(960, 720), "ABOUT", Style::Default);

					int x = mainMenu.MainMenuPressed();
					if (x == 0)
					{
						while (Play.isOpen())
						{
							Event aevent;
							while (Play.pollEvent(aevent))
							{
								if (aevent.type == Event::Closed)
									Play.close();
								if (aevent.type == Event::KeyPressed)
								{
									if (aevent.key.code == Keyboard::Escape)
										Play.close();
								}
							}
							OPTIONS.close();
							ABOUT.close();
							Play.clear();
							Play.display();
						}
					}
					if (x == 1)
					{
						while (OPTIONS.isOpen())
						{
							Event aevent;
							while (OPTIONS.pollEvent(aevent))
							{
								if (aevent.type == Event::Closed)
									OPTIONS.close();
								if (aevent.type == Event::KeyPressed)
								{
									if (aevent.key.code == Keyboard::Escape)
										OPTIONS.close();
								}
							}
							Play.close();
							OPTIONS.close();
							ABOUT.clear();

							OPTIONS.display();
						}
					}
					if (x == 2) {
						while (ABOUT.isOpen())
						{
							Event aevent;
							while (ABOUT.pollEvent(aevent))
							{
								if (aevent.type == Event::Closed)
									ABOUT.close();
								if (aevent.type == Event::KeyPressed)
								{
									if (aevent.key.code == Keyboard::Escape)
										ABOUT.close();
								}
							}
							Play.close();
							OPTIONS.clear();
							ABOUT.clear();
							ABOUT.display();
						}
					}
					if (x == 3) {
						MENU.close();
						break;
					}
				}
			}
		}
	}
	MENU.clear();
	mainMenu.draw(MENU);
	MENU.display();


	// Initialise random number generator
	srand((unsigned)time(NULL));
	// Initialise a unique GLUT session
	GlutSession::GetInstance().Init(argc, argv);
	// Create a new asteroids game
	Asteroids asteroids(argc, argv);
	// Start the asteroids game
	asteroids.Start();

	return 0;
}

