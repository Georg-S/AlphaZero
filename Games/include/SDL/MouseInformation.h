#ifndef PROTOTYPE_MOUSEINFORMATION_H
#define PROTOTYPE_MOUSEINFORMATION_H

struct MouseInformation {
    MouseInformation() {
        this->xPosition = 0;
        this->yPosition = 0;
        this->leftButtonPressed = false;
        this->rightButtonPressed = false;
    }

    MouseInformation(int xPosition, int yPosition, bool leftClicked) {
        this->xPosition = xPosition;
        this->yPosition = yPosition;
        this->leftButtonPressed = leftClicked;
    }

    int xPosition;
    int yPosition;
    bool leftButtonPressed;
    bool rightButtonPressed;
};

#endif //PROTOTYPE_MOUSEINFORMATION_H
