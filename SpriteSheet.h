#pragma once

//TODO: spritesheets need to also record their framerate and adjust based on it

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

#include "tinyxml2.h"

class Animation;
class Sprite;
class SpriteSheet;

//XML MetaData File + PNG Spritesheet
//Spritesheet contains individual frames of animation ordered in rows of a set x by y size
//The spritesheets are set that animations start at a row/col and continue to the right until done
//The MetaData for the SpriteSheet indicates the x by y size of the sprite squares and the filepath of the MetaData and the PNG
//The MetaData for the animations give the name, starting row/col, and number of frames, as well as if it should repeat or not
class SpriteSheet
{
public:
	SpriteSheet(SDL_Renderer* Renderer, std::string FilePath); //Framerate? or just store that in the xml aswell?
	~SpriteSheet();
	bool RequestAnimation(std::string Name);
	void Update();

	void RenderSprite(int X = 0, int Y = 0);

	//XML save/load
	bool Save();
	bool Load();

	//TestingFunctions //TODO: remove
	void InitDummyFile();
	void TestRender(SDL_Surface * GlobalSurface, SDL_Renderer * Renderer);

private:
	SDL_Renderer* m_Renderer = nullptr;
	std::string m_FilePath = "Test";
 
	//Expl. I want the game to run at 60fps, but I know this sheet normally plays at 24fps, so I need to accomodate a 24fps sprite in a 60fps application
	//The theory is by getting delta times in render I can keep an alternate track of time, so while the main loop is calling render 60fps the sprite is only changin to the next sprite 24fps
	int m_FrameRate = 5;
	double m_ElapsedTime = 0;
	double m_MSDelay = 0; //Calculated from m_FrameRate

	int m_XFactor = 0, m_YFactor = 0; //The size in pixels each sprite is on the spritesheet (uniform through the sheet)
	SDL_Surface* m_Surface = nullptr;
	SDL_Texture* m_SpriteTexture = nullptr;

	Animation* m_CurrentAnimation = nullptr;
	std::vector<Animation> m_Animations = std::vector<Animation>();

	std::string GetXMLFilePath() { return m_FilePath + ".xml"; }
	std::string GetPNGFilePath() { return m_FilePath + ".png"; }
	
	void Init();
	void CalcMSDelay(); //16.7ish 60fps, 33.3ish 30fps,  41.7ish 24fps (rounded to ints)
};

//SpriteSheets are composed of a number of sprite rects of a fixes size, x by y,
//Animations only know the starting row/col and the number of frames they get before repeating or stopping at the last frame.
//Should be savable to xml
//TODO: this ordering is strange
class Animation
{
public:
	Animation(std::string Name, bool Repeats, int StartRow, int StartCol, int FrameCount);
	void UpdateFrame(bool Start = false);
	bool IsAnimationFinished();
	SDL_Rect GetSprite(int XFactor, int YFactor);

	std::string GetName() { return m_Name; }
	void Save(tinyxml2::XMLDocument *Doc, tinyxml2::XMLNode *RootNode);
	static Animation Load(tinyxml2::XMLDocument *Doc, tinyxml2::XMLNode *RootNode);

private:
	std::string m_Name = "DefaultAnimationName";
	bool m_Repeats = false;
	int m_StartRow, m_StartCol = 0;
	int m_FrameCount = 0;
	int m_CurrentFrame = 0;
};
