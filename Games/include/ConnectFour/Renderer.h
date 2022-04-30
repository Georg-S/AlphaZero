#ifndef DEEPREINFORCEMENTLEARNING_cn4_RENDERER_H
#define DEEPREINFORCEMENTLEARNING_cn4_RENDERER_H


#include "SDL/SDLHandler.h"
#include "GameLogic.h"

namespace cn4
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer() = default;
		void update(const Board& board);
		void quit();
		bool isQuit() const;
		int getWindowXPosition() const;
		int getWindowYPosition() const;
		int windowWidth() const;
		int windowHeight() const;

	private:
		static constexpr int m_windowWidth = 700;
		static constexpr int m_windowHeight = 600;
		void renderPiece(const Board& board, int x, int y);
		int convertBoardXPositionToRenderXPosition(const Board& board, int x);
		int convertBoardYPositionToRenderYPosition(const Board& board, int y);

		std::unique_ptr<SDLHandler> handler;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_cn4_RENDERER_H
