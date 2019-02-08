#pragma once

namespace RenderImages
{
	class PNGImage : public IUpdatable
	{
	public:
		PNGImage(SDL_Renderer *Renderer, std::string FilePath)
			: IUpdatable()
			, m_Renderer(Renderer)
			, m_FilePath(FilePath)
		{
			LoadImage();
		}

		~PNGImage()
		{
			m_Renderer = nullptr;
			SDL_FreeSurface(m_Surface);
			SDL_DestroyTexture(m_Texture);
		}

		void SetDrawPosition(int x = 0, int y = 0)
		{
			m_XOffset = static_cast<float>(x);
			m_YOffset = static_cast<float>(y);
		}

		void Render() override
		{
			if (!m_Renderer || !m_Surface || !m_Texture) return;

			auto desRect = SDL_Rect(m_Surface->clip_rect);
			desRect.x = static_cast<int>(std::round(m_XOffset));
			desRect.y = static_cast<int>(std::round(m_YOffset));

			SDL_RenderCopy(m_Renderer, m_Texture, &m_Surface->clip_rect, &desRect);
		}

	private:
		SDL_Renderer *m_Renderer = nullptr;
		SDL_Surface *m_Surface = nullptr;
		SDL_Texture *m_Texture = nullptr;

		std::string m_FilePath;
		float m_XOffset, m_YOffset = 0;

		void LoadImage()
		{
			if (!m_Renderer || IMG_Init(IMG_INIT_PNG) != 2) return;

			m_Surface = IMG_Load((m_FilePath + ".png").c_str());
			if (!m_Surface) return;

			if (m_Texture)
				SDL_DestroyTexture(m_Texture);

			m_Texture = SDL_CreateTextureFromSurface(m_Renderer, m_Surface);
			SDL_SetTextureBlendMode(m_Texture, SDL_BLENDMODE_BLEND);
		}
	};
}