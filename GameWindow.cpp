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
	gWindow = SDL_CreateWindow("SDL Game Window (Insert Pun Here)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_Width, m_Height, SDL_WINDOW_SHOWN);
	if (!gWindow) return false;

	//Get window surface
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	//Create renderer for window
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	m_Initialized = true;
	return true;
}

void GameWindow::Main()
{
	//Init the Delta Timer (attaches self to Updater and starts calculating time since last call)
	DeltaTimer::GetInstance();
	auto ExitHandler = EHandlers::ProgramExitHandler(&m_Exiting); //Handles Escape + top right X application exiting by modifying the bQuit bool

	auto BackgroundImage = RenderImages::PNGImage(gRenderer, "./Images/base");
	auto PlayerCharacter = Character(gRenderer, "./Images/SpriteSheets/MainCharacterSpriteSheet_56x56");

#ifdef Debugging
	auto test = UpdatableClassTest();
#endif

	int x = 0, y = 0;
	int i = 0;
	auto Events = std::vector<SDL_Event>();
	while (true)
	{
		if (m_Exiting) break;

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


		BackgroundImage.SetDrawPosition(x, y);
		x++; y--;
		i++;
	}

	Free();
}

void GameWindow::Free()
{
	//Deallocate surface
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = nullptr;

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

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
