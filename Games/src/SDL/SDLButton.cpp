#include "SDL/SDLButton.h"



SDLButton::SDLButton(SDLHandler* sdlHandler, int x, int y, int width, int height, std::string unselectedFilePath, std::string selectedFilePath)
{
    this->sdlHandler = sdlHandler;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->unselectedFilePath = unselectedFilePath;
    this->selectedFilePath = selectedFilePath;

    selected = sdlHandler->createAndPushBackRenderElement(selectedFilePath, x, y, width, height);
    unselected = sdlHandler->createAndPushBackRenderElement(unselectedFilePath, x, y, width, height);
    selected->render = false;
}

SDLButton::~SDLButton()
{
}

void SDLButton::updateButton(const MouseInformation &mouseInformation)
{
    if (isMouseOverButton(mouseInformation.xPosition, mouseInformation.yPosition)) {
        selected->render = true;
        unselected->render = false;
        this->clicked = mouseInformation.leftButtonPressed;
    }
    else {
        selected->render = false;
        unselected->render = true;
    }
}

bool SDLButton::wasClicked()
{
    return clicked;
}

void SDLButton::destroy()
{
    sdlHandler->deleteRenderingElement(selected);
    sdlHandler->deleteRenderingElement(unselected);
}

bool SDLButton::isMouseOverButton(const int & mousePositionX, const int & mousePositionY)
{
    if (mousePositionX >= this->x && mousePositionX <= this->x + this->width) {
        if (mousePositionY >= this->y && mousePositionY <= this->y + this->height)
            return true;
    }
    return false;
}