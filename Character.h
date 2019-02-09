#pragma once

#include <SDL.h>
#include <vector>

#include "Update.h"
#include "SpriteSheet.h"
#include "PhysicsVectors.h"
#include "PhysicsObjects.h"

//derived class PlayerCharacter to be added later
//derived class AI character to be added later possibly
class Character : public IEventHandler //TODO: I swear this name is used by something and it will come back to haunt me
{
public: 
	Character(SDL_Renderer* Renderer, std::string SpriteSheetPath, std::string PhysicsManager);
	void Update() override;
	void Render() override;
	void HandleEvents(std::vector<SDL_Event> * Events) override;
	void HandleEvent(SDL_Event * Event) override;

private:
	SpriteSheet m_SpriteSheet;
	Vector2f m_Offset = Vector2f(0,0); 
	PhysicsObject m_Physics;
};
