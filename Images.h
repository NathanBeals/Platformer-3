#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Update.h"

namespace RenderImages
{
	class PNGImage : public IUpdatable
	{
	public:
		PNGImage(SDL_Renderer *Renderer, std::string FilePath);

		~PNGImage();

		void SetDrawPosition(int x = 0, int y = 0);

		void Render() override;

	private:
		SDL_Renderer *m_Renderer = nullptr;
		SDL_Surface *m_Surface = nullptr;
		SDL_Texture *m_Texture = nullptr;

		std::string m_FilePath;
		float m_XOffset, m_YOffset = 0;

		void LoadImage();
	};
}