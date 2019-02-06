/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//TODO: work on this enough so it's not just the lazyfood tutorial
//Removing comments on things that will likely be removed or moved elsewhere
//TODO: Remove all these gross globals
//TODO: many of these comments are from the lazyfoo tutorial and won't make sense in the application, re-evaluate comments when moved

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

#include "tinyxml2.h"
#include <memory>

#include "SpriteSheet.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

//TODO: Tutorial functions to be elevated
bool init();
bool loadMedia();
void close();

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;
SDL_Renderer* gRenderer = NULL;

//SpriteSheet SpriteSheetTests();

bool init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	//Create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	//Get window surface
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	//Create renderer for window
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	return true;
}


//TODO: Remove/rework, non-applicaple to final product
bool loadMedia()
{
	//Load splash image
	gHelloWorld = SDL_LoadBMP("Images/base.bmp");
	if (gHelloWorld == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", "Images/base.bmp", SDL_GetError());
		return false;
	}

	return true;
}

void close()
{
	//Deallocate surface
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
	IMG_Quit();
}

namespace
{
	//HACK: Well, huh... I have a gut feeling this method is a complete wash but huh, re-evaluate later
	//My quest to remove the nesting of the tutorial continues
	bool EarlyExitOnTrue(bool Condition, std::string FailureString = "")
	{
		if (!Condition) return Condition;
		printf(FailureString.c_str());
		close();
		return Condition;
	}
}

void MainLoop();

int main(int argc, char* args[])
{
	MainLoop();
	close(); //If this doesn't get called it means I'm a bad person
	return 0; //Moving to a void function makes this return value rather meaningless does it not?
}

//TODO: name too cheeky?
//TODO: Jesus a lot of these functions can fail returning -1, Checking them all would be the right thing to do but boy do I not want to
void MainLoop()
{
	if (EarlyExitOnTrue(!init(), "Failure to Init")) return;
	if (EarlyExitOnTrue(!loadMedia(), "Failure to Load Media")) return;

	//is 0 true or false? I can never remember
	//false == 0
	if (EarlyExitOnTrue(SDL_SetRenderDrawColor(gRenderer, 0x30, 0xFF, 0xFF, 0xFF), "Failure to Change Render Color")) return;
	if (EarlyExitOnTrue(SDL_RenderClear(gRenderer), "Failure to Clear Renderer")) return;
	if (EarlyExitOnTrue(SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL), "Blit Test Failed")) return;
	if (EarlyExitOnTrue(SDL_UpdateWindowSurface(gWindow), "Update Windows Surface Failed")) return;

	//Tests for sprite sheet animations
	auto sheet = SpriteSheet(gRenderer, "C://Users//beals_000//source//repos//Project1/Project1/HelloWorld");
	//auto sheet = SpriteSheetTests(sheet); //can fail, but not like the others
	sheet.RequestAnimation("Testa2");

	SDL_RenderPresent(gRenderer); //can't fail? interesting

	//This is a bonkers loop
	int i = 0;
	while (i < 100)
	{
		SDL_RenderClear(gRenderer);
		i++;
		SDL_Delay(40);
		sheet.RenderSprite(50, 50);
		sheet.Update();
		SDL_RenderPresent(gRenderer); //can't fail? interesting
	}

	//Wait two seconds
	SDL_Delay(1000);
}

//void SpriteSheetTests()
//{
//	//auto sheet = SpriteSheet(gRenderer, "C://Users//beals_000//source//repos//Project1/Project1/HelloWorld");
//	//sheet.InitDummyFile(); //Overwrites loaded file
//	//sheet.Load();
//	//sheet.Save(); //order
//	//sheet.TestRender(gScreenSurface, gRenderer);
//	sheet.RequestAnimation("Testa2");
//	sheet.RenderSprite(50, 50);
//
//	return sheet;
//}