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

Character::Character(SDL_Renderer* Renderer, std::string SpriteSheetPath, PhysicsManager *PhysicsManager)
	: IEventHandler()
	, m_SpriteSheet(Renderer, SpriteSheetPath)
	, m_Physics(PhysicsManager, CreateSimpleBoxCollider(0,0,26,26), 100.0f, &m_Offset)
{

}

void Character::Update()
{
	//TODO: simplify
	//HACK: simplify, also this upsets me
	//Held down keys
	auto v = m_Physics.GetVelocity();
	auto vAmount = 100.f;
	auto keyStates = SDL_GetKeyboardState(NULL);
	//if (!keyStates[SDL_SCANCODE_W] && !keyStates[SDL_SCANCODE_S] && !keyStates[SDL_SCANCODE_A] && !keyStates[SDL_SCANCODE_D])
	//	m_ApplyFrictionThisFrame = true;
	if (keyStates[SDL_SCANCODE_W])
	{
		v.y = -vAmount;
		if (v.x > 0)
			m_SpriteSheet.RequestAnimation("JumpR");
		else
			m_SpriteSheet.RequestAnimation("JumpL");
	}
	if (keyStates[SDL_SCANCODE_S])
	{
		v.y = vAmount;
		if (v.x > 0)
			m_SpriteSheet.RequestAnimation("FallR");
		else
			m_SpriteSheet.RequestAnimation("FallL");
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