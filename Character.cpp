#include "Character.h"
#include <math.h>
#include <algorithm>

Character::Character(SDL_Renderer* Renderer, std::string SpriteSheetPath)
	: IEventHandler()
	, m_SpriteSheet(Renderer, SpriteSheetPath)
	, m_SimpleCollisionBox(SDL_Rect())
{
	//TODO: consider how you want collision to function
	m_SimpleCollisionBox.x = 0;
	m_SimpleCollisionBox.y = 0;
	m_SimpleCollisionBox.w = 26;
	m_SimpleCollisionBox.h = 26;
}

void Character::Update()
{
	//TODO: simplify
	//HACK: simplify, also this upsets me
	//Held down keys
	m_ApplyFrictionThisFrame = false;
	auto vAmount = 100.f;
	auto keyStates = SDL_GetKeyboardState(NULL);
	if (!keyStates[SDL_SCANCODE_W] && !keyStates[SDL_SCANCODE_S] && !keyStates[SDL_SCANCODE_A] && !keyStates[SDL_SCANCODE_D])
		m_ApplyFrictionThisFrame = true;
	if (keyStates[SDL_SCANCODE_W])
	{
		m_YVelocity = -vAmount;
		if (m_XVelocity > 0)
			m_SpriteSheet.RequestAnimation("JumpR");
		else
			m_SpriteSheet.RequestAnimation("JumpL");
	}
	if (keyStates[SDL_SCANCODE_S])
	{
		m_YVelocity = vAmount;
		if (m_XVelocity > 0)
			m_SpriteSheet.RequestAnimation("FallR");
		else
			m_SpriteSheet.RequestAnimation("FallL");
	}
	if (keyStates[SDL_SCANCODE_A])
	{
		m_XVelocity = -vAmount;
		m_SpriteSheet.RequestAnimation("Left");
	}
	if (keyStates[SDL_SCANCODE_D])
	{
		m_XVelocity = vAmount;
		m_SpriteSheet.RequestAnimation("Right");
	}

	m_SpriteSheet.Update();
	m_X += m_XVelocity * static_cast<float>((DeltaTimer::GetDeltaTime() / 1000.0)); //TODO: deltatime returns milisecond results, may needs some fine tuning
	m_Y += m_YVelocity * static_cast<float>((DeltaTimer::GetDeltaTime() / 1000.0));

	if (m_ApplyFrictionThisFrame)
		ApplyFriction();

	if (std::abs(m_XVelocity) < 1.0f) m_XVelocity = 0.0f;
	if (std::abs(m_YVelocity) < 1.0f) m_YVelocity = 0.0f;
}

void Character::Render()
{
	m_SpriteSheet.RenderSprite(static_cast<int>(std::round(m_X)), static_cast<int>(std::round(m_Y)));
}

void Character::HandleEvents(std::vector<SDL_Event>* Events)
{
	IEventHandler::HandleEvents(Events);
}

void Character::HandleEvent(SDL_Event * Event)
{

}

void Character::ApplyFriction()
{
	m_XVelocity /= 1.2f;
	m_YVelocity /= 1.2f;

	m_ApplyFrictionThisFrame = false;
}