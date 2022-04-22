#ifndef DEEPREINFORCEMENTLEARNING_MOUSE_H
#define DEEPREINFORCEMENTLEARNING_MOUSE_H

#include <iostream>
#include <SDL.h>
#include "MouseInformation.h"

class Mouse
{
public:
	Mouse() = default;
	void update();
	bool isLeftPressed();
	bool isRightPressed();
	bool isNewLeftClick();
	void getMousePosition(int& x, int& y);
	int getMousePositionX();
	int getMousePositionY();

private:
	bool lastTickLeftClicked = false;
	bool newLeftClick = false;
	MouseInformation mouseInfo{};
};

#endif //DEEPREINFORCEMENTLEARNING_MOUSE_H
