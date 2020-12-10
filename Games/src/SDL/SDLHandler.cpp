#include "SDL/SDLHandler.h"

SDLHandler::SDLHandler(int screenWidth, int screenHeight, bool useCaching)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	this->useCaching = useCaching;
}

bool SDLHandler::start(const std::string& windowName)
{
	if (!initialize(windowName)) {
		printf("Could not initialize\n");
		return false;
	}
	return true;
}

RenderingElement* SDLHandler::createAndPushBackRenderElement(std::string fileName, int x, int y, int width, int height)
{
	RenderingElement* element = new RenderingElement;
	element->texture = createAndReturnTexture(fileName);
	element->transform = SDL_Rect{ x,y,width, height };


	if (element->texture != NULL) {
		elements.push_back(element);
	}
	else
		std::cout << "Problem " << std::endl;
	return element;
}

void SDLHandler::changePositionOfRenderingElement(RenderingElement* element, int x, int y)
{
	element->transform.x = x - element->transform.w / 2;
	element->transform.y = y - element->transform.h / 2;
}

void SDLHandler::close()
{
	clear();
	deleteCache();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void SDLHandler::deleteCache()
{
	for (auto x : cache)
		SDL_DestroyTexture(x.second);
}

void SDLHandler::setToForeground(RenderingElement* element)
{
	int index = getIndex(element);
	if (index != -1)
		std::swap(elements[elements.size() - 1], elements[index]);
}

void SDLHandler::updateRendering()
{
	updateQuit();
	SDL_RenderClear(renderer);

	for (int i = 0; i < elements.size(); i++) {
		if (elements[i]->render)
			SDL_RenderCopy(renderer, elements[i]->texture, NULL, &elements[i]->transform);
	}

	SDL_RenderPresent(renderer);
}

SDL_Texture* SDLHandler::createAndReturnTexture(std::string fileName)
{
	if (useCaching)
	{
		if (cache.find(fileName) != cache.end())
			return cache[fileName];
	}

	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(fileName.c_str());

	if (loadedSurface == NULL)
		printf("Unable to load the image %s! SDL_image Error: %s\n", fileName.c_str(), IMG_GetError());
	else {
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

		if (newTexture == NULL)
			printf("Unable to create the texture from %s! SDL_Error: %s\n", fileName.c_str(), SDL_GetError());

		SDL_FreeSurface(loadedSurface);
	}

	if (useCaching)
		this->cache[fileName] = newTexture;

	return newTexture;
}

void SDLHandler::clear() {
	if (useCaching)
	{
		for (RenderingElement* element : elements)
			delete element;
		elements.clear();
	}
	else
	{
		for (RenderingElement* element : elements)
		{
			deleteRenderingElementAndTexture(element);
		}
	}
}

void SDLHandler::deleteRenderingElementAndTexture(RenderingElement* element)
{
	int index = getIndex(element);
	if (index == -1)
		return;

	elements.erase(elements.begin() + index);

	SDL_DestroyTexture(element->texture);
	delete element;
}

RenderingElement* SDLHandler::createAndPushFrontRenderElement(std::string fileName, int x, int y, int width, int height)
{
	RenderingElement* element = new RenderingElement;
	element->texture = createAndReturnTexture(fileName);
	element->transform = SDL_Rect{ x,y,width, height };

	if (element->texture != NULL) {
		elements.insert(elements.begin(), element);
	}
	return element;
}

bool SDLHandler::initialize(const std::string& windowName)
{
	return initializeVideo() && initializeWindow(windowName) && initializeRenderer() && initializeImageFlags() && initializeTime();
}

bool SDLHandler::initializeVideo()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	return true;
}

bool SDLHandler::initializeWindow(const std::string& windowName)
{
	window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screenWidth, m_screenHeight, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

bool SDLHandler::initializeRenderer()
{
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		printf("We were not able to create the renderer! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	return true;
}

bool SDLHandler::initializeImageFlags()
{
	int imgFlags = IMG_INIT_PNG;

	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_image could not be initialized! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	return true;
}

int SDLHandler::getIndex(RenderingElement* element)
{
	int index = -1;
	for (int i = 0; i < elements.size(); i++) {
		if (elements[i] == element)
			index = i;
	}
	return index;
}

bool SDLHandler::initializeTime()
{
	startTime = SDL_GetTicks();
	return true;
}

void SDLHandler::getWindowPosition(int* x, int* y) {
	SDL_GetWindowPosition(window, x, y);
}

void SDLHandler::updateQuit() {
	while (SDL_PollEvent(&event) != 0) {
		if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) || event.type == SDL_QUIT)
			exit = true;
	}
}
