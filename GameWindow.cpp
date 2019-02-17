#pragma once

#include "GameWindow.h"
#include "Update.h"
#include "EventHandlers.h"
#include "Character.h"
#include "Images.h"
#include <algorithm>

GameWindow::GameWindow(int Width, int Height)
	: m_Width(Width)
	, m_Height(Height)
{

}

GameWindow::~GameWindow()
{
	Free(); //TODO: Free maybe not the best name
}

bool GameWindow::Start()
{
	if (m_Initialized) Free();
	if (EarlyExitOnTrue(!Init(), "Failure to Init Game Window")) return false;

	Main();
	return true;
}

bool GameWindow::Init()
{
	//Init SDL Video
	if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;

	//Create Window
	m_Window = SDL_CreateWindow("SDL Game Window (Insert Pun Here)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_Width, m_Height, SDL_WINDOW_SHOWN);
	if (!m_Window) return false;

	//Get window surface
	m_ScreenSurface = SDL_GetWindowSurface(m_Window);

	//Create renderer for window
	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(m_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	m_Initialized = true;
	return true;
}

void GameWindow::Main()
{
	//Init the Delta Timer (attaches self to Updater and starts calculating time since last call)
	DeltaTimer::GetInstance();
	auto foregroundPhysicsManager = PhysicsManager();
	//auto backgroundPhysicsManager = PhysicsManager();
	auto exitHandler = EHandlers::ProgramExitHandler(&m_Exiting); //Handles Escape + top right X application exiting by modifying the bQuit bool

	auto backgroundImage = RenderImages::PNGImage(m_Renderer, "./Images/base");
	auto playerCharacter = Character(m_Renderer, "./Images/SpriteSheets/MainCharacterSpriteSheet_56x56", &foregroundPhysicsManager, CreateSimpleBoxCollider(0,0, 56, 56), 20); //TODO: offsets don't work, you should know why


	//Cubes
	auto floor = RenderImages::SimpleImageObject(m_Renderer, "./Images/floor", &foregroundPhysicsManager, CreateSimpleBoxCollider(0, 0, 400, 50), 5000);
	floor.SetKinematic(true);
	floor.SetStatic(true);
	floor.SetOffset(Vector2f(50, 800));

	auto wallL = RenderImages::SimpleImageObject(m_Renderer, "./Images/Wall50x400", &foregroundPhysicsManager, CreateSimpleBoxCollider(0, 0, 50, 400), 5000);
	wallL.SetKinematic(true);
	wallL.SetStatic(true);
	wallL.SetOffset(Vector2f(0, 400));

	auto wallR = RenderImages::SimpleImageObject(m_Renderer, "./Images/Wall50x400", &foregroundPhysicsManager, CreateSimpleBoxCollider(0, 0, 50, 400), 5000);
	wallR.SetKinematic(true);
	wallR.SetStatic(true);
	wallR.SetOffset(Vector2f(450, 400));

	auto randomCube2 = RenderImages::SimpleImageObject(m_Renderer, "./Images/test50", &foregroundPhysicsManager, CreateSimpleBoxCollider(0, 0, 50, 50), 100);
	randomCube2.SetOffset(Vector2f(150, 100));
	auto randomCube3 = RenderImages::SimpleImageObject(m_Renderer, "./Images/test50", &foregroundPhysicsManager, CreateSimpleBoxCollider(0, 0, 50, 50), 10);
	randomCube3.SetOffset(Vector2f(280, 200));


	//Test class that just prints out deltatimes
	//auto test = UpdatableClassTest();

	auto events = std::vector<SDL_Event>();
	while (true)
	{
		//Told to Exit
		if (m_Exiting) break;

		//Handle Events (SDL Input)
		events.clear();
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
			events.push_back(SDL_Event(e));

		//Generate Updates
		UpdateGenerator::GetInstance().HandleEvents(&events);
		UpdateGenerator::GetInstance().Update();
		UpdateGenerator::GetInstance().Render();
		SDL_RenderPresent(m_Renderer); 
	}

	Free();
}

void GameWindow::Free()
{
	//Destroy window
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_DestroyRenderer(m_Renderer);
	m_Renderer = nullptr;

	//Exit subsystems //TODO: check if these should be in Free or in ~GameWindow, not sure what reiniting them does
	SDL_Quit();
	IMG_Quit();

	//Pesky Pointers
	m_Initialized = false;
}

//TODO: give this helper a home.

bool GameWindow::EarlyExitOnTrue(bool Condition, std::string FailureString)
{
	if (!Condition) return Condition;
	printf(FailureString.c_str());
	Free();
	return Condition;
}
