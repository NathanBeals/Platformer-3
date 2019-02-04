#pragma once

class Base
{
private:
	int hello = 0;
	int whatever = 0;
	int gooble = 0;

	//Screen dimension constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

public:
	int SDL_main(int argc, char* args[]);
};