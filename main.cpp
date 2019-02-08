/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//TODO: work on this enough so it's not just the lazyfood tutorial
//Removing comments on things that will likely be removed or moved elsewhere
//TODO: Remove all these gross globals
//TODO: many of these comments are from the lazyfoo tutorial and won't make sense in the application, re-evaluate comments when moved


#define Debugging 0; //HACK: Not DEBUG, pushing off setting up Release

#include <SDL.h>
#include <stdio.h>
#include <memory>
#include <algorithm>

#include "tinyxml2.h"
#include "Update.h"
#include "SpriteSheet.h"
#include "EventHandlers.h"
#include "Character.h"

//Applications will attempt to exit when true
bool bQuit = false;

//Window
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

//TODO: Tutorial functions to be elevated
bool Init();
bool LoadMedia();
void Close();

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;
SDL_Renderer* gRenderer = NULL;

class GameWindow;

bool EarlyExitOnTrue(bool Condition, std::string FailureString = "")
{
	if (!Condition) return Condition;
	printf(FailureString.c_str());
	Close();
	return Condition;
}

bool Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
	gWindow = SDL_CreateWindow("SDL Game Window (Insert Pun Here)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!gWindow) return false;

	//Get window surface
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	//Create renderer for window
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	return true;
}

//TODO: Remove/rework, non-applicaple to final product
bool LoadMedia()
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

void Close()
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

void MainLoop();

int main(int argc, char* args[])
{
	if (EarlyExitOnTrue(!Init(), "Failure to Init")) return 1;
	if (EarlyExitOnTrue(!LoadMedia(), "Failure to Load Media")) return 1;
	if (EarlyExitOnTrue(SDL_SetRenderDrawColor(gRenderer, 0x30, 0xFF, 0xFF, 0xFF), "Failure to Change Render Color")) return 1;
	if (EarlyExitOnTrue(SDL_RenderClear(gRenderer), "Failure to Clear Renderer")) return 1;
	if (EarlyExitOnTrue(SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL), "Blit Test Failed")) return 1;
	if (EarlyExitOnTrue(SDL_UpdateWindowSurface(gWindow), "Update Windows Surface Failed")) return 1;

	MainLoop();
	Close();
	return 0;
}

void SpriteSheetTests();

void MainLoop()
{
	//Init the Delta Timer (attaches self to Updater and starts calculating time since last call)
	DeltaTimer::GetInstance();
	auto ExitHandler = EHandlers::ProgramExitHandler(&bQuit); //Handles Escape + top right X application exiting by modifying the bQuit bool

	auto PlayerCharacter = Character(gRenderer, "./Images/SpriteSheets/MainCharacterSpriteSheet_56x56");
	
#ifdef Debugging
	auto test = UpdatableClassTest();
#endif // Debugging

	//TODO: more consideration should be used for this updating logic, it's important after all
	int i = 0;
	auto Events = std::vector<SDL_Event>();
	while (true)
	{
		if (bQuit) break;

		//TODO: re-evaluate this chunk, it's hard to read, as is most of main (base)
		Events.clear();
		SDL_Event e;

		while (SDL_PollEvent(&e) != 0)
			Events.push_back(SDL_Event(e));

		UpdateGenerator::GetInstance().HandleEvents(&Events);
		UpdateGenerator::GetInstance().Update();
		UpdateGenerator::GetInstance().Render();

		SDL_RenderPresent(gRenderer); //can't fail? interesting

		SDL_Delay(40); //TODO: consider consistent framerates?

		i++;
	}

	//if (!bQuit)
	//	SpriteSheetTests();
}
