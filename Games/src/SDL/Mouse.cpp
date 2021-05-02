#include "SDL/Mouse.h"

Mouse::Mouse()
{

}

void Mouse::update()
{
	SDL_PumpEvents();
	Uint32 mouseState = SDL_GetMouseState(&mouseInfo.xPosition, &mouseInfo.yPosition);
	if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		mouseInfo.leftButtonPressed = true;

		if (lastTickLeftClicked)
		{
			newLeftClick = false;
		}
		else
		{
			newLeftClick = true;
			lastTickLeftClicked = true;
		}
	}
	else
	{
		lastTickLeftClicked = false;
		newLeftClick = false;
	}

	mouseInfo.rightButtonPressed = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

bool Mouse::isNewLeftClick()
{
	return newLeftClick;
}

bool Mouse::isLeftPressed()
{
	return mouseInfo.leftButtonPressed;
}

void Mouse::getMousePosition(int& x, int& y)
{
	x = mouseInfo.xPosition;
	y = mouseInfo.yPosition;
}

int Mouse::getMousePositionX()
{
	return mouseInfo.xPosition;
}

int Mouse::getMousePositionY()
{
	return mouseInfo.yPosition;
}

bool Mouse::isRightPressed()
{
	return mouseInfo.rightButtonPressed;
}
