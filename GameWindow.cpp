#pragma once

#include "GameWindow.h"
#include "Update.h"
#include "EventHandlers.h"
#include "Character.h"
#include "Images.h"

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

//TODO: why did you make all of the local variables here capped?
//TODO: undo the capping of all these local variables
void GameWindow::Main()
{
	//Init the Delta Timer (attaches self to Updater and starts calculating time since last call)
	DeltaTimer::GetInstance();
	auto ForegroundPhysicsManager = PhysicsManager();
	auto ExitHandler = EHandlers::ProgramExitHandler(&m_Exiting); //Handles Escape + top right X application exiting by modifying the bQuit bool

	auto BackgroundImage = RenderImages::PNGImage(m_Renderer, "./Images/base");
	auto PlayerCharacter = Character(m_Renderer, "./Images/SpriteSheets/MainCharacterSpriteSheet_56x56", &ForegroundPhysicsManager);
	auto RandomCube = RenderImages::SimpleImageObject(m_Renderer, "./Images/test50", &ForegroundPhysicsManager);
	//Test class that just prints out deltatimes
	auto test = UpdatableClassTest();

	auto Events = std::vector<SDL_Event>();
	while (true)
	{
		//Told to Exit
		if (m_Exiting) break;

		//Handle Events (SDL Input)
		Events.clear();
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
			Events.push_back(SDL_Event(e));

		//Generate Updates
		UpdateGenerator::GetInstance().HandleEvents(&Events);
		UpdateGenerator::GetInstance().Update();
		UpdateGenerator::GetInstance().Render();
		SDL_RenderPresent(m_Renderer); 

		//HACK: Lock FrameRate...... uhhhhh TODO: what am I doing here
		auto dTime = DeltaTimer::GetDeltaTime();
		auto res = static_cast<int>(std::round(dTime / 1000.0)) % 20; //% 20.0;
		SDL_Delay(20 - res); //TODO: consider consistent framerates? //60ish fps, I think vsync is on so it'll cap at 60 unless I don't want it to
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
