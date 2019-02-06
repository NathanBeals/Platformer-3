#include "SpriteSheet.h"
#include <algorithm>

SpriteSheet::SpriteSheet(SDL_Renderer* Renderer, std::string FilePath)
	: m_Renderer(Renderer)
	, m_FilePath(FilePath)
{
	Init();
}

SpriteSheet::~SpriteSheet()
{
	m_Renderer = nullptr; 
	SDL_FreeSurface(m_OptiSurface);
	SDL_DestroyTexture(m_SpriteTexture);
}

bool SpriteSheet::RequestAnimation(std::string Name)
{
	auto anim = std::find_if(std::begin(m_Animations), std::end(m_Animations), [&](auto a){ return a.GetName().compare(Name) == 0; });
	if (anim == std::end(m_Animations)) return false;

	m_CurrentAnimation = &*anim;
	m_CurrentAnimation->UpdateFrame(true);

	return true;
}

void SpriteSheet::Update()
{
	if (!m_CurrentAnimation) return;
	m_CurrentAnimation->UpdateFrame();
}

void SpriteSheet::RenderSprite(int X, int Y)
{
	if (!m_Renderer || !m_CurrentAnimation) return;

	auto spriteRect = m_CurrentAnimation->GetSprite(m_XFactor, m_YFactor);

	//Center it
	auto desRect = SDL_Rect();
	desRect.x = X - m_XFactor / 2;
	desRect.y = Y - m_YFactor / 2;
	desRect.h = m_XFactor;
	desRect.w = m_YFactor;

	SDL_RenderCopy(m_Renderer, m_SpriteTexture, &spriteRect, &desRect);
}

void SpriteSheet::InitDummyFile()
{
	m_XFactor = 32;
	m_YFactor = 32;
	m_Animations.clear();
	m_Animations.push_back(Animation("Testa1", false, 0, 0, 5));
	m_Animations.push_back(Animation("Testa2", true, 1, 0, 5));
	m_Animations.push_back(Animation("Testa3", false, 1, 5, 10));
}

//TODO: look at simplifying this area, possibly
void SpriteSheet::Init()
{
	//XML File load / Saving
	if (Load())
		printf(("SpriteSheet loaded from file, " + m_FilePath).c_str());
	else if (Save())
		printf(("Creating file for new SpriteSheet, " + m_FilePath).c_str());
	else
		printf(("SpriteSheet failed to load or save, " + m_FilePath).c_str());

	//Iamge File loading
	auto loadedFlag = IMG_Init(IMG_INIT_PNG);
	m_OptiSurface = IMG_Load(GetPNGFilePath().c_str());

	if (loadedFlag == 0 || !m_OptiSurface)
		printf(("Image failed to load, " + m_FilePath).c_str());

	//TODO: move to init
	if (m_SpriteTexture)
		SDL_DestroyTexture(m_SpriteTexture);
	m_SpriteTexture = SDL_CreateTextureFromSurface(m_Renderer, m_OptiSurface);
	SDL_SetTextureBlendMode(m_SpriteTexture, SDL_BLENDMODE_BLEND); //can fail
}

void SpriteSheet::TestRender(SDL_Surface * GlobalSurface, SDL_Renderer * Renderer)
{
	SDL_Texture* newtex = SDL_CreateTextureFromSurface(Renderer, m_OptiSurface); //can fail
	SDL_SetTextureBlendMode(newtex, SDL_BLENDMODE_BLEND); //can fail

	auto myrect = SDL_Rect();
	myrect.x = 0;
	myrect.y = 0;
	myrect.h = 500;
	myrect.w = 500;

	SDL_RenderCopy(Renderer, newtex, &myrect, &myrect);

	if (newtex != NULL)
		SDL_DestroyTexture(newtex);
}

bool SpriteSheet::Save()
{
	/* XML File Format
	XFactor
	YFactor
	Animations
		Animation
			Name
			Repeats
			StartRow
			StartCol
			Frames
	*/

	using namespace tinyxml2;

	XMLDocument xmlDoc;
	auto *pRoot = xmlDoc.NewElement("Root");
	xmlDoc.InsertFirstChild(pRoot);

	//XFactor, YFactor
	auto *pNXFactor = xmlDoc.NewElement("XFactor");
	pNXFactor->SetText(m_XFactor);
	pRoot->InsertEndChild(pNXFactor);
	auto *pNYFactor = xmlDoc.NewElement("YFactor");
	pNYFactor->SetText(m_YFactor);
	pRoot->InsertEndChild(pNYFactor);

	//Animations
	auto *pNAnimations = xmlDoc.NewElement("Animations");
	pRoot->InsertEndChild(pNAnimations);
	for (auto x : m_Animations)
	{
		auto *pNAnim = xmlDoc.NewElement("Animation");
		pNAnimations->InsertEndChild(pNAnim);
		x.Save(&xmlDoc, pNAnim);
	}

	XMLError eResult = xmlDoc.SaveFile(GetXMLFilePath().c_str());
	return eResult == 0;
}

bool SpriteSheet::Load()
{
	using namespace tinyxml2;

	XMLDocument xmlDoc;
	auto EResult = xmlDoc.LoadFile(GetXMLFilePath().c_str());
	if (EResult != XMLError::XML_SUCCESS) return false;

	auto *pRoot = xmlDoc.FirstChildElement("Root");

	auto *pXFactor = pRoot->FirstChildElement("XFactor");
	auto *pYFactor = pRoot->FirstChildElement("YFactor");
	auto *pAnimations = pRoot->FirstChildElement("Animations");
	if (!pXFactor || !pYFactor || !pAnimations) {}
	else
	{
		//SpriteSheet
		pXFactor->QueryIntText(&m_XFactor);
		pYFactor->QueryIntText(&m_YFactor);

		//Animations
		auto *pAnim = pAnimations->FirstChildElement("Animation");

		m_Animations.clear();
		while (pAnim != nullptr)
		{
			m_Animations.push_back(Animation::Load(&xmlDoc, pAnim));
			pAnim = pAnim->NextSiblingElement("Animation");
		}
	}

	return true;
}


//SpriteSheets are split into a number of rectangles of a set size, x by y
//Animations are given the x y of their spritesheet so they can calculate their bidness

Animation::Animation(std::string Name, bool Repeats, int StartRow, int StartCol, int FrameCount) :
	m_Name(Name),
	m_Repeats(Repeats),
	m_StartRow(StartRow),
	m_StartCol(StartCol),
	m_FrameCount(FrameCount)
{

}

void Animation::UpdateFrame(bool Start)
{
	m_CurrentFrame++;
	if (m_CurrentFrame >= m_FrameCount)
	{
		if (m_Repeats) m_CurrentFrame = 0;
		else m_CurrentFrame = m_FrameCount - 1;
	}

	if (Start)
		m_CurrentFrame = 0;
}

bool Animation::IsAnimationFinished()
{
	return !m_Repeats && m_CurrentFrame == m_FrameCount - 1;
}

//HACK: ok now i'm starting to see that x and y may be confusing
SDL_Rect Animation::GetSprite(int XFactor, int YFactor)
{
	auto newRect = SDL_Rect();
	newRect.w = XFactor;
	newRect.h = YFactor;
	newRect.x = XFactor * (m_StartCol + m_CurrentFrame);
	newRect.y = YFactor * (m_StartRow);

	return newRect;
}

void Animation::Save(tinyxml2::XMLDocument *Doc, tinyxml2::XMLNode * RootNode)
{
	if (Doc == nullptr || RootNode == nullptr) return;

	auto *n = Doc->NewElement("Name");
	n->SetText(m_Name.c_str());
	RootNode->InsertEndChild(n);

	auto *r = Doc->NewElement("Repeats");
	r->SetText(m_Repeats);
	RootNode->InsertEndChild(r);

	auto *sr = Doc->NewElement("StartRow");
	sr->SetText(m_StartRow);
	RootNode->InsertEndChild(sr);

	auto *sc = Doc->NewElement("StartCol");
	sc->SetText(m_StartCol);
	RootNode->InsertEndChild(sc);

	auto *fc = Doc->NewElement("FrameCount");
	fc->SetText(m_FrameCount);
	RootNode->InsertEndChild(fc);
}

Animation Animation::Load(tinyxml2::XMLDocument * Doc, tinyxml2::XMLNode * RootNode)
{
	std::string name = "Failed";
	bool bRepeats = false;
	int sRow = 0;
	int sCol = 0;
	int frameCount = 0;

	if (Doc != nullptr && RootNode != nullptr)
	{
		auto *n = RootNode->FirstChildElement("Name");
		auto *r = RootNode->FirstChildElement("Repeats");
		auto *sr = RootNode->FirstChildElement("StartRow");
		auto *sc = RootNode->FirstChildElement("StartCol");
		auto *fc = RootNode->FirstChildElement("FrameCount");

		//TODO: add more and better failure checks
		if (!n || !r || !sr || !sc || !fc) {}
		else
		{
			name = n->GetText();
			r->QueryBoolText(&bRepeats);
			sr->QueryIntText(&sRow);
			sc->QueryIntText(&sCol);
			fc->QueryIntText(&frameCount);
		}
	}

	return Animation(name, bRepeats, sRow, sCol, frameCount);
}
