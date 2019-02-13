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

//TODO: rework completely
void Character::Update()
{
	if (m_HMoving)
	{
		if (m_CurHDirection == Right)
			m_Physics.ApplyForce(m_HMovementForce * m_Physics.GetWeight() * DeltaTimer::GetDeltaTime() / 1000, 0);

		if (m_CurHDirection == Left)
			m_Physics.ApplyForce(-m_HMovementForce * m_Physics.GetWeight() * DeltaTimer::GetDeltaTime() / 1000, 0);
	}

	//Animation block?
	if (m_Jumping && m_Physics.GetVelocity().y > 0)
	{
		m_Jumping = false;
		m_Falling = true;
		m_CurHDirection == Right ? m_SpriteSheet.RequestAnimation("FallR") : m_SpriteSheet.RequestAnimation("FallL");
	}

	//HACK: this need to activate when the character lands on something, because of how I'm moved the collisions away, this could prove, interesting
	else if (!m_Jumping && m_Falling && m_Physics.GetVelocity().y > -3 && m_Physics.GetVelocity().y < 3)
	{
		m_Falling = false;
		m_JumpCounter = 0;
		m_CurHDirection == Right ? m_SpriteSheet.RequestAnimation("LandR") : m_SpriteSheet.RequestAnimation("LandL");
	}

	else if (abs(m_Physics.GetVelocity().x) <= 50 && !m_Falling && !m_Jumping) //TODO: arbitrary value
	{
		m_CurHDirection == Right ? m_SpriteSheet.RequestAnimation("IdleR") : m_SpriteSheet.RequestAnimation("IdleL");
	}

	else if (abs(m_Physics.GetVelocity().x) > 50 && !m_Falling && !m_Jumping)
	{
		m_CurHDirection == Right ? m_SpriteSheet.RequestAnimation("Right") : m_SpriteSheet.RequestAnimation("Left");
	}

	m_SpriteSheet.Update();
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
	if (Event->type == SDL_KEYDOWN)
	{
		if (Event->key.keysym.sym == SDLK_w)
			Jump();

		if (Event->key.keysym.sym == SDLK_s)
		{
			//Crouch
		}

		if (Event->key.keysym.sym == SDLK_a)
		{
			m_HMoving = true;
			Move(Left);
		}

		if (Event->key.keysym.sym == SDLK_d)
		{
			m_HMoving = true;
			Move(Right);
		}
	}
	if (Event->type == SDL_KEYUP)
	{
		if (Event->key.keysym.sym == SDLK_a && m_CurHDirection == Left)
		{
			m_HMoving = false;
			//Move(Stopped);
		}

		if (Event->key.keysym.sym == SDLK_d && m_CurHDirection == Right)
		{
			m_HMoving = false;
			//Move(Stopped);
		}
	}
}

void Character::SetOffset(Vector2f Location)
{
	m_Offset = Location;
	m_Physics.SetOffset(Location);
}

void Character::Jump()
{
	if (m_MaxJumps < m_JumpCounter) return;

	m_JumpCounter++;
	m_Physics.ApplyForce(0, m_JumpForce * m_Physics.GetWeight());
	m_Jumping = true;
	m_Falling = false;

	m_CurHDirection == Right ? m_SpriteSheet.RequestAnimation("JumpR") : m_SpriteSheet.RequestAnimation("JumpL");
}

//TODO: rename

void Character::Move(HDirection M)
{
	m_PrevHDirection = m_CurHDirection;
	m_CurHDirection = M;
}
