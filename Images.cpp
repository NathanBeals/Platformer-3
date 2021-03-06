#include "Images.h"

RenderImages::PNGImage::PNGImage(SDL_Renderer * Renderer, std::string FilePath)
	: IUpdatable()
	, m_Renderer(Renderer)
	, m_FilePath(FilePath)
{
	LoadImage();
}

RenderImages::PNGImage::~PNGImage()
{
	m_Renderer = nullptr;
	SDL_FreeSurface(m_Surface);
	SDL_DestroyTexture(m_Texture);
}

void RenderImages::PNGImage::SetDrawPosition(int x, int y)
{
	m_Offset.x = static_cast<float>(x);
	m_Offset.y = static_cast<float>(y);
}

void RenderImages::PNGImage::Render()
{
	if (!m_Renderer || !m_Surface || !m_Texture) return;

	auto desRect = SDL_Rect(m_Surface->clip_rect);
	desRect.x = static_cast<int>(std::round(m_Offset.x));
	desRect.y = static_cast<int>(std::round(m_Offset.y));

	SDL_RenderCopy(m_Renderer, m_Texture, &m_Surface->clip_rect, &desRect);
}

void RenderImages::PNGImage::LoadImage()
{
	if (!m_Renderer || IMG_Init(IMG_INIT_PNG) != 2) return;

	m_Surface = IMG_Load((m_FilePath + ".png").c_str());
	if (!m_Surface) return;

	if (m_Texture)
		SDL_DestroyTexture(m_Texture);

	m_Texture = SDL_CreateTextureFromSurface(m_Renderer, m_Surface);
	SDL_SetTextureBlendMode(m_Texture, SDL_BLENDMODE_BLEND);
}
