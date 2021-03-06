#ifndef DEEPREINFORCEMENTLEARNING_SDLHANDLER_H
#define DEEPREINFORCEMENTLEARNING_SDLHANDLER_H

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <map>


struct RenderingElement
{
	SDL_Rect transform;
	SDL_Texture* texture;
	bool render = true;
};

class SDLHandler
{
public:
	SDLHandler(int screenWidth, int screenHeight, bool useCaching = false);
	bool start(const std::string& windowName);
	void updateRendering();
	std::shared_ptr<RenderingElement> createAndPushFrontRenderElement(std::string fileName, int x, int y, int width, int height);
	std::shared_ptr<RenderingElement> createAndPushBackRenderElement(std::string fileName, int x, int y, int width, int height);
	void clear();
	void deleteRenderingElementAndTexture(std::shared_ptr<RenderingElement> element);
	static void changePositionOfRenderingElement(std::shared_ptr<RenderingElement> element, int x, int y);
	void close();
	void setToForeground(std::shared_ptr<RenderingElement> element);
	void getWindowPosition(int* x, int* y);
	void updateQuit();

	bool exit = false;
	SDL_Event event;

private:
	void deleteCache();
	SDL_Texture* createAndReturnTexture(std::string fileName);
	bool initialize(const std::string& windowName);
	bool initializeVideo();
	bool initializeWindow(const std::string& windowName);
	bool initializeRenderer();
	bool initializeTime();
	bool initializeImageFlags();
	int getIndex(std::shared_ptr<RenderingElement> element);

	std::vector<std::shared_ptr<RenderingElement>> elements;
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	int m_screenWidth;
	int m_screenHeight;
	uint32_t startTime;
	bool useCaching = false;
	std::map<std::string, SDL_Texture*>cache;
};


#endif //DEEPREINFORCEMENTLEARNING_SDLHANDLER_H
