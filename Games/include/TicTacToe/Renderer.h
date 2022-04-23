#ifndef DEEPREINFORCEMENTLEARNING_ttt_RENDERER_H
#define DEEPREINFORCEMENTLEARNING_ttt_RENDERER_H

#include <memory>
#include "SDL/SDLHandler.h"
#include "GameLogic.h"

namespace ttt
{
	class Renderer
	{
	public:
		Renderer();
		void update(const ttt::Board& board);
		void quit();
		bool isQuit() const;
		int windowWidth() const;
		int windowHeight() const;

	private:
		std::unique_ptr<SDLHandler> m_sdlHandler;
		static constexpr int m_windowWidth = 800;
		static constexpr int m_windowHeight = 800;
		static constexpr int pieceWidth = m_windowWidth / 3;
		static constexpr int pieceHeight = m_windowHeight / 3;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_ttt_RENDERER_H
