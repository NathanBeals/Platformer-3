#include "EventHandlers.h"

EHandlers::ProgramExitHandler::ProgramExitHandler(bool * Exit)
	: IEventHandler()
	, m_Quit(Exit)
{

}

void EHandlers::ProgramExitHandler::HandleEvent(SDL_Event * Event)
{
	if (Event->type == SDL_QUIT || (Event->type == SDL_KEYUP && Event->key.keysym.sym == SDLK_ESCAPE))
		*m_Quit = true;
}
