#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(std::string FilePath)
	: m_FilePath(FilePath)
{
	//Load(FilePath);
}

SpriteSheet::~SpriteSheet()
{
	SDL_FreeSurface(m_OptiSurface);
}

bool SpriteSheet::RequestAnimation(std::string Name)
{
	auto anim = GetAnimation(Name);
	if (anim == nullptr) return false;

	m_CurrentAnimation = anim;
	m_CurrentAnimation->UpdateFrame(true);

	return true;
}

void SpriteSheet::Update()
{
	if (m_CurrentAnimation == nullptr) return;
	m_CurrentAnimation->UpdateFrame();
}

Animation * SpriteSheet::GetAnimation(std::string Name)
{
	for (auto x : m_Animations)
		if (Name == x.GetName())
			return &x;

	return nullptr;
}

void SpriteSheet::InitDummyFile()
{
	m_Animations.push_back(Animation("Testa1", false, 0, 0, 5));
	m_Animations.push_back(Animation("Testa2", true, 1, 0, 5));
	m_Animations.push_back(Animation("Testa3", false, 1, 5, 10));
}

void SpriteSheet::Init(SDL_Surface* GlobalSurface)
{
	auto dingus = IMG_Init(IMG_INIT_PNG);
	SDL_Surface* tempLoadSurface = IMG_Load(GetPNGFilePath().c_str());
	if (tempLoadSurface == NULL) return;

	SDL_FreeSurface(m_OptiSurface);
	m_OptiSurface = SDL_ConvertSurface(tempLoadSurface, GlobalSurface->format, NULL);

	SDL_FreeSurface(tempLoadSurface);
}

void SpriteSheet::TestRender(SDL_Surface * GlobalSurface, SDL_Renderer * Renderer)
{
	SDL_Texture* newtex = SDL_CreateTextureFromSurface(Renderer, m_OptiSurface);
	SDL_SetTextureBlendMode(newtex, SDL_BLENDMODE_BLEND);
	//SDL_BlitSurface(m_OptiSurface, NULL, GlobalSurface, NULL);
	
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
	/*
	XFactor
	YFactor
	Animations
	Name
	Repeats //TODO: maybe don't store this?
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
	xmlDoc.LoadFile(GetXMLFilePath().c_str()); //TODO: failure check
	
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

//TODO: Simplify block
Vector2 Animation::UpdateFrame(bool Start)
{
	if (m_CurrentFrame == m_FrameCount - 1)
	{
		if (m_Repeats)
			m_CurrentFrame = 0;
	}
	else
		m_CurrentFrame++;

	if (Start)
		m_CurrentFrame = 0;

	return Vector2(m_StartRow, m_StartCol + m_CurrentFrame);
}

bool Animation::IsAnimationFinished()
{
	return !m_Repeats && m_CurrentFrame == m_FrameCount - 1;
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
		if (!n || !r || !sr || !sc || !fc){}
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
