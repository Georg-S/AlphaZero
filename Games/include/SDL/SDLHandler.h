#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <utility>


struct RenderingElement
{
	RenderingElement() = default;
	RenderingElement(SDL_Rect transform, SDL_Texture* texture)
		: transform(std::move(transform))
		, texture(texture)
	{
	};

	SDL_Rect transform = {};
	SDL_Texture* texture = nullptr;
	bool render = true;
};

class SDLTextureWrapper
{
public:
	SDLTextureWrapper() = delete;
	SDLTextureWrapper(const SDLTextureWrapper&) = delete;
	SDLTextureWrapper& operator=(const SDLTextureWrapper&) = delete;
	SDLTextureWrapper(SDL_Texture* texture) : m_texture(texture) {};
	~SDLTextureWrapper() { SDL_DestroyTexture(m_texture); }
	SDL_Texture* texture() const { return m_texture; };
private:
	SDL_Texture* m_texture = nullptr;
};

class SDLHandler
{
public:
	SDLHandler(int screenWidth, int screenHeight, bool useCaching = true);
	bool start(const std::string& windowName);
	void update();
	bool createAndPushBackRenderElement(const std::string& fileName, int x, int y, int width, int height);
	void clear();
	void close();
	bool isExit() const;
	std::pair<int, int> getWindowPosition() const;

private:
	void updateQuit();
	void updateRendering();
	void deleteCache();
	SDL_Texture* createAndReturnTexture(std::string fileName);
	bool initialize(const std::string& windowName);
	bool initializeVideo();
	bool initializeWindow(const std::string& windowName);
	bool initializeRenderer();
	bool initializeTime();
	bool initializeImageFlags();

	std::vector<std::unique_ptr<RenderingElement>> m_elements;
	SDL_Window* m_window = NULL;
	SDL_Renderer* m_renderer = NULL;
	int m_screenWidth;
	int m_screenHeight;
	bool m_useCaching = false;
	uint32_t m_startTime;
	bool m_exit = false;
	std::map<std::string, std::unique_ptr<SDLTextureWrapper>> m_cache;
};
