#pragma once

#include "Update.h"

namespace EHandlers
{
	class ProgramExitHandler : public IEventHandler
	{
	public:
		ProgramExitHandler(bool *Exit);
		void HandleEvent(SDL_Event * Event) override;

	private:
		bool *m_Quit;
	};
}