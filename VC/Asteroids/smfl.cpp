#include "smfl.h"
#include <SFML/Graphics.hpp>
#include "MainMenu.h"
using namespace sf;

void main()
{
	// Make a Main Window
	RenderWindow window(VideoMode(960, 720), "Main Menu", Style::Default);
	MainMenu menu(MENU.getSize().x, MENU.getSize().y);

	while (MENU.isOpen())
	{
		Event event;
		while (MENU.pollEvent(event))
		{
			if (event.type == Event::Closed)
				MENU.close();
			if (event.type == Event::Released)
			{
				if (event.key.code == Keyboard::Up)
					menu.MoveUp();
					break;
				if (event.key.code == Keyboard::Down)
					menu.MoveDown();
					break;
				if (event.key.code == Keyboard::Return)
				{
					RenderWindow Play(VideoMode(960, 720), "Asteroidsss", Style::Default);
					RenderWindow OPTIONS(VideoMode(960, 720), "OPTIONS", Style::Default);
					RenderWindow ABOUT(VideoMode(960, 720), "ABOUT", Style::Default);

					int x = MainMenuPressed();
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

}