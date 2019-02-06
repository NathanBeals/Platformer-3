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

void SpriteSheetTests();

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
	bool EarlyExitOnTrue(bool Condition, std::string FailureString)
	{
		auto c = Condition;
		if (c)
		{
			printf(FailureString.c_str());
			close();
		}
		return c;
	}
}

//HACK: ??? the nested ifs were driving me nuts so I put it in a self ending while loop, I'm sure there's a better way but this solves my most immediate and irksome problem
int main(int argc, char* args[])
{
	//Start up SDL and create window

	if (EarlyExitOnTrue(!init(), "Failure to Init")) return 0;
	if (EarlyExitOnTrue(!loadMedia(), "Failure to Load Media")) return 0;

	SDL_SetRenderDrawColor(gRenderer, 0x30, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);

	SDL_UpdateWindowSurface(gWindow);

	//HACK: relying on gScreenSurface to not be nullptr
	SpriteSheetTests();

	SDL_RenderPresent(gRenderer);

	//Wait two seconds
	SDL_Delay(2000);

	close();
	return 0;
}

void MainLoop()
{

}

void SpriteSheetTests()
{
	auto sheet = SpriteSheet("C://Users//beals_000//source//repos//Project1/Project1/HelloWorld");
	sheet.Init(gScreenSurface);
	sheet.InitDummyFile();
	sheet.Save();
	sheet.Load();
	sheet.TestRender(gScreenSurface, gRenderer);
	SDL_UpdateWindowSurface(gWindow);
}