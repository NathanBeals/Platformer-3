#pragma once

#include <SDL.h>
#include <string>

//After doing all the work to put this together, I am uncertain of the value of moving this from main
class GameWindow
{
public:
	GameWindow(int Width, int Height);
	~GameWindow();

	bool Start();
	bool Init();

private:
	bool m_Initialized = false;
	bool m_Exiting = false;
	int m_Width, m_Height = 0;

	SDL_Window* gWindow = nullptr;
	SDL_Surface* gScreenSurface = nullptr;
	SDL_Surface* gHelloWorld = nullptr;
	SDL_Renderer* gRenderer = nullptr;

	void Main();
	void Free();

	//TODO: give this helper a home.
	bool EarlyExitOnTrue(bool Condition, std::string FailureString = "");
};


