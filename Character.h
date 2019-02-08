#pragma once

#include <SDL.h>
#include <vector>

#include "Update.h"
#include "SpriteSheet.h"

//derived class PlayerCharacter to be added later
//derived class AI character to be added later possibly
class Character : public IEventHandler //TODO: I swear this name is used by something and it will come back to haunt me
{
public: 

	Character(SDL_Renderer* Renderer, std::string SpriteSheetPath);
	void Update() override;
	void Render() override;
	void HandleEvents(std::vector<SDL_Event> * Events) override;
	void HandleEvent(SDL_Event * Event) override;

private:
	SpriteSheet m_SpriteSheet;
	SDL_Rect m_SimpleCollisionBox;
	float m_X, m_Y = 0; 
	float m_XVelocity, m_YVelocity = 0; //left to right, up to down, negative to positive
	float m_Gravity = 9.8f; //HACK: unsure if some of these values should be floats or not, I'm pretty sure they should be

	bool m_ApplyFrictionThisFrame = false;
	void ApplyFriction();
};
