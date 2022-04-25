#ifndef DEEPREINFORCEMENTLEARNING_MOUSE_H
#define DEEPREINFORCEMENTLEARNING_MOUSE_H

#include <iostream>
#include <SDL.h>
#include <utility>

class Mouse
{
public:
	Mouse() = default;
	void update();
	bool isLeftPressed() const;
	bool isRightPressed() const;
	bool isNewLeftClick() const;
	std::pair<int, int> getMousePosition() const;
	int getMousePositionX() const;
	int getMousePositionY() const;

private:
	bool m_lastTickLeftClicked = false;
	bool m_newLeftClick = false;
	int m_xPosition = 0;
	int m_yPosition = 0;
	bool m_leftButtonPressed = false;
	bool m_rightButtonPressed = false;
};

#endif //DEEPREINFORCEMENTLEARNING_MOUSE_H
