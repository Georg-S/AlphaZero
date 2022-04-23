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
	int m_xPosition;
	int m_yPosition;
	bool m_leftButtonPressed;
	bool m_rightButtonPressed;
};

#endif //DEEPREINFORCEMENTLEARNING_MOUSE_H
