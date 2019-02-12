#pragma once

#include "Character.h"
#include <math.h>
#include <algorithm>

//Helpers to construct the collider lists needed by PhysicObjects
namespace 
{
	std::vector<SDL_Rect> CreateSimpleBoxCollider(int x, int y, int w, int h)
	{
		auto result = std::vector<SDL_Rect>();
		SDL_Rect collider;
		collider.x = x;
		collider.y = y;
		collider.w = w;
		collider.h = h;
		result.push_back(collider);

		return result;
	}
}

Character::Character(SDL_Renderer* Renderer, std::string SpriteSheetPath, PhysicsManager *PhysicsManager, std::vector<SDL_Rect> Colliders = std::vector<SDL_Rect>(), float Weight = 10.0f)
	: IEventHandler()
	, m_SpriteSheet(Renderer, SpriteSheetPath)
	, m_Physics(PhysicsManager, Colliders, Weight, &m_Offset) 
{

}

//HACK: this whole section is a mess from testing, move friction to physics objects, and split animations from input
void Character::Update()
{
	auto v = m_Physics.GetVelocity();

	//TODO: Rework after expanding the spritesheet, and adding more physics objects (like the floor)
	//TODO: Friction, move to collision area
	v.x /= 1.1;
	v.y /= 1.1;
	if (v.y > 0 && v.y < 1) v.y = 0;
	if (v.x > 0 && v.x < 1) v.x = 0;

	auto vAmount = 100.f; //TODO: make class member
	auto keyStates = SDL_GetKeyboardState(NULL);
	if (keyStates[SDL_SCANCODE_W])
	{
		v.y = -vAmount;
	}
	if (keyStates[SDL_SCANCODE_S])
	{
		v.y = vAmount;
	}
	if (keyStates[SDL_SCANCODE_A])
	{
		v.x = -vAmount;
		m_SpriteSheet.RequestAnimation("Left");
	}
	if (keyStates[SDL_SCANCODE_D])
	{
		v.x = vAmount;
		m_SpriteSheet.RequestAnimation("Right");
	}

	if (v.y < 0)
	{
		if (v.x > 0)
			m_SpriteSheet.RequestAnimation("JumpR");
		else
			m_SpriteSheet.RequestAnimation("JumpL");
	}
	else if (v.y > 0)
	{
		if (v.x > 0)
			m_SpriteSheet.RequestAnimation("FallR");
		else
			m_SpriteSheet.RequestAnimation("FallL");
	}

	if (v.x == 0 && v.y == 0)
	{
		//TODO: LdleL, IdleR
		//m_SpriteSheet.getcurrentanimationname() == "" //enum of right animations??
		m_SpriteSheet.RequestAnimation("IdleR");
	}

	//v.y += 9.8f; //TODO: either move to the physicsobject or make a class member;

	m_Physics.SetVelocity(v);
	m_SpriteSheet.Update();

	//m_X += m_XVelocity * static_cast<float>((DeltaTimer::GetDeltaTime() / 1000.0)); //TODO: deltatime returns milisecond results, may needs some fine tuning
	//m_Y += m_YVelocity * static_cast<float>((DeltaTimer::GetDeltaTime() / 1000.0));

	//if (std::abs(m_XVelocity) < 1.0f) m_XVelocity = 0.0f;
	//if (std::abs(m_YVelocity) < 1.0f) m_YVelocity = 0.0f;
}

void Character::Render()
{
	m_SpriteSheet.RenderSprite(static_cast<int>(std::round(m_Physics.GetOffset().x)), static_cast<int>(std::round(m_Physics.GetOffset().y)));
}

void Character::HandleEvents(std::vector<SDL_Event>* Events)
{
	IEventHandler::HandleEvents(Events);
}

void Character::HandleEvent(SDL_Event * Event)
{

}