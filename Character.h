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

	void SetOffset(Vector2f Location);

	PhysicsObject *GetPhysicsObject() { return &m_Physics; } //TODO: hack?

	enum HDirection
	{
		Right,
		Left,
		Stopped
	};

	void Jump();
	void Move(HDirection M);	//TODO: rename

private:
	//TODO: order
	SpriteSheet m_SpriteSheet;
	Vector2f m_Offset = Vector2f(0,0); 
	PhysicsObject m_Physics;
	int m_MaxJumps = 2;
	int m_JumpCounter = 0;
	bool m_Jumping = false;
	bool m_Falling = false;
	float m_JumpForce = -980; //Responds to keydown not update so independent of delta time
	bool m_HMoving = false;
	HDirection m_PrevHDirection = Stopped;
	HDirection m_CurHDirection = Stopped;
	float m_HMovementForce = 1000;

	bool m_Floored = true; //Touching Floor, TODO: figure a way to update this signifigantly
};
