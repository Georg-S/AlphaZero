#include "SDL/SDLHandler.h"

SDLHandler::SDLHandler(int screenWidth, int screenHeight, bool useCaching)
	: m_screenWidth(screenWidth)
	, m_screenHeight(screenHeight)
	, m_useCaching(useCaching)
	, m_startTime(0)
{
}

bool SDLHandler::start(const std::string& windowName)
{
	if (!initialize(windowName))
	{
		printf("Could not initialize\n");
		return false;
	}
	return true;
}

void SDLHandler::update()
{
	updateQuit();
	updateRendering();
}

bool SDLHandler::createAndPushBackRenderElement(const std::string& fileName, int x, int y, int width, int height)
{
	SDL_Texture* texture = createAndReturnTexture(fileName);

	if (!texture) 
	{
		std::cout << "Problem creating rendering element " << std::endl;
		return false;
	}

	m_elements.emplace_back(std::make_unique<RenderingElement>(SDL_Rect{ x,y,width, height }, texture ));

	return true;
}

void SDLHandler::close()
{
	clear();
	deleteCache();
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

bool SDLHandler::isExit() const
{
	return m_exit;
}

void SDLHandler::deleteCache()
{
	m_cache.clear();
}

void SDLHandler::updateRendering()
{
	updateQuit();
	SDL_RenderClear(m_renderer);

	for (const auto& elem : m_elements)
	{
		if (elem->render)
			SDL_RenderCopy(m_renderer, elem->texture, NULL, &elem->transform);
	}

	SDL_RenderPresent(m_renderer);
}

SDL_Texture* SDLHandler::createAndReturnTexture(std::string fileName)
{
	if (m_useCaching)
	{
		if (m_cache.find(fileName) != m_cache.end())
			return m_cache.at(fileName)->texture();
	}

	SDL_Texture* newTexture = nullptr;
	SDL_Surface* loadedSurface = IMG_Load(fileName.c_str());

	if (!loadedSurface) 
	{
		printf("Unable to load the image %s! SDL_image Error: %s\n", fileName.c_str(), IMG_GetError());
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(m_renderer, loadedSurface);

		if (!newTexture)
			printf("Unable to create the texture from %s! SDL_Error: %s\n", fileName.c_str(), SDL_GetError());

		SDL_FreeSurface(loadedSurface);
	}

	if (m_useCaching && newTexture)
		m_cache[fileName] = std::make_unique<SDLTextureWrapper>(newTexture);

	return newTexture;
}

void SDLHandler::clear()
{
	if (!m_useCaching)
	{
		for (auto& element : m_elements)
			SDL_DestroyTexture(element->texture);
	}

	m_elements.clear();
}

bool SDLHandler::initialize(const std::string& windowName)
{
	return initializeVideo() && initializeWindow(windowName) && initializeRenderer() && initializeImageFlags() && initializeTime();
}

bool SDLHandler::initializeVideo()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	return true;
}

bool SDLHandler::initializeWindow(const std::string& windowName)
{
	m_window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screenWidth, m_screenHeight, SDL_WINDOW_OPENGL);
	if (!m_window)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

bool SDLHandler::initializeRenderer()
{
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if (!m_renderer)
	{
		printf("We were not able to create the renderer! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

	return true;
}

bool SDLHandler::initializeImageFlags()
{
	constexpr int imgFlags = IMG_INIT_PNG;

	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not be initialized! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	return true;
}

bool SDLHandler::initializeTime()
{
	m_startTime = SDL_GetTicks();
	return true;
}

std::pair<int, int> SDLHandler::getWindowPosition() const
{
	int x = 0;
	int y = 0;
	SDL_GetWindowPosition(m_window, &x, &y);

	return { x, y };
}

void SDLHandler::updateQuit()
{
	SDL_Event event = {};
	//	Keep in mind that this call also seems to handle QT Events -> multiple game windows can be opened but this leads to problems
	while (SDL_PollEvent(&event) != 0) 
	{
		if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) || event.type == SDL_QUIT)
			m_exit = true;
	}
}
