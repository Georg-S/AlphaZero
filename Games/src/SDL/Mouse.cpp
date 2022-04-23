#include "SDL/Mouse.h"

void Mouse::update()
{
	SDL_PumpEvents();
	Uint32 mouseState = SDL_GetMouseState(&m_xPosition, &m_yPosition);
	if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		m_leftButtonPressed = true;

		if (m_lastTickLeftClicked)
		{
			m_newLeftClick = false;
		}
		else
		{
			m_newLeftClick = true;
			m_lastTickLeftClicked = true;
		}
	}
	else
	{
		m_lastTickLeftClicked = false;
		m_newLeftClick = false;
	}

	m_rightButtonPressed = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

bool Mouse::isNewLeftClick() const
{
	return m_newLeftClick;
}

bool Mouse::isLeftPressed() const
{
	return m_leftButtonPressed;
}

std::pair<int, int> Mouse::getMousePosition() const
{
	return { m_xPosition, m_yPosition };
}

int Mouse::getMousePositionX() const
{
	return m_xPosition;
}

int Mouse::getMousePositionY() const
{
	return m_yPosition;
}

bool Mouse::isRightPressed() const
{
	return m_rightButtonPressed;
}
