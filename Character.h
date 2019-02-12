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
	Character(SDL_Renderer* Renderer, std::string SpriteSheetPath, PhysicsManager *PhysicsManager, std::vector<SDL_Rect> Colliders, float Weight);
	void Update() override;
	void Render() override;
	void HandleEvents(std::vector<SDL_Event> * Events) override;
	void HandleEvent(SDL_Event * Event) override;

	void SetOffset(Vector2f Location)
	{
		m_Offset = Location;
		m_Physics.SetOffset(Location);
	}

private:
	SpriteSheet m_SpriteSheet;
	Vector2f m_Offset = Vector2f(0,0); 
	PhysicsObject m_Physics;
};
