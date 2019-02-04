#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include "tinyxml2.h"

//TODO: Add guards for nullptrs

class Animation;
class Sprite;
class SpriteSheet;

struct Vector2
{
	int m_X, m_Y = 0;
	Vector2(int X, int Y) :
		m_X(X),
		m_Y(Y) {}
};

//XML MetaData File + PNG Spritesheet
//Spritesheet contains individual frames of animation ordered in rows of a set x by y size
//The spritesheets are set that animations start at a row/col and continue to the right until done
//The MetaData for the SpriteSheet indicates the x by y size of the sprite squares and the filepath of the MetaData and the PNG
//The MetaData for the animations give the name, starting row/col, and number of frames, as well as if it should repeat or not
class SpriteSheet
{
public:
	SpriteSheet(std::string FilePath);
	bool RequestAnimation(std::string Name);
	void Update();

private:
	std::string m_FilePath = "Test";
	int m_XFactor, m_YFactor = 0; //The size in pixels each sprite is on the spritesheet (uniform through the sheet)

	Animation* m_CurrentAnimation = nullptr;
	std::vector<Animation> m_Animations = std::vector<Animation>();
	Animation* GetAnimation(std::string Name);

	//TODO: Unify naming, pN(XXXX) Pointer+Node but considering they're all local, makes it hard to read (pick one)
	bool Save();

	//TODO: remove maybe?
	std::string GetXMLFilePath() { return m_FilePath + ".png"; }
	std::string GetPNGFilePath() { return m_FilePath + ".xml"; }
};

//SpriteSheets are composed of a number of sprite rects of a fixes size, x by y,
//Animations only know the starting row/col and the number of frames they get before repeating or stopping at the last frame.
//Should be savable to xml
class Animation
{
public:
	Animation(std::string Name, bool Repeats, int StartRow, int StartCol, int FrameCount);
	Vector2 UpdateFrame(bool Start = false);
	bool IsAnimationFinished();


	std::string GetName() { return m_Name; }
	void Save(tinyxml2::XMLDocument *Doc, tinyxml2::XMLNode *RootNode);

	//void LoadFromXml();

private:
	std::string m_Name = "dAnim";
	bool m_Repeats = false;
	int m_StartRow, m_StartCol = 0;
	int m_FrameCount = 0;
	int m_CurrentFrame = 0;
};



