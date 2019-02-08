#define Debugging 0; //HACK: Not DEBUG, pushing off setting up Release

#include <stdio.h>
#include "GameWindow.h"

//Window
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

int main(int argc, char* args[])
{
	auto MainWin = GameWindow(SCREEN_HEIGHT, SCREEN_WIDTH);
	if (MainWin.Start() == false) printf("Game Window Failed to Start.");
	return 0;
}