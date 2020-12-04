#ifndef PROTOTYPE_SDLBUTTON_H
#define PROTOTYPE_SDLBUTTON_H


#include <string>
#include "SDLHandler.h"
#include "MouseInformation.h"

class SDLButton
{
public:
    SDLButton(SDLHandler* sdlHandler, int x, int y, int width, int height, std::string unselectedFilePath, std::string selectedFilePath);
    ~SDLButton();
    void updateButton(const MouseInformation &mouseInformation);
    bool wasClicked();
    void destroy();
private:
    bool isMouseOverButton(const int &mousePositionX, const int &mousePositionY);

    SDLHandler* sdlHandler;
    RenderingElement* unselected;
    RenderingElement* selected;
    bool clicked = false;

    int x;
    int y;
    int width;
    int height;
    std::string unselectedFilePath;
    std::string selectedFilePath;
};


#endif //PROTOTYPE_SDLBUTTON_H
