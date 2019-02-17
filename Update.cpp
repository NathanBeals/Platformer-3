#include "Update.h"
#include <algorithm>
#include <SDL.h>

UpdateGenerator & UpdateGenerator::GetInstance()
{
	static UpdateGenerator instance;
	return instance;
}

void UpdateGenerator::HandleEvents(std::vector<SDL_Event>* Events)
{
	for (auto x : m_Children)
		if (auto ptr = dynamic_cast<IEventHandler*>(&*x))
			ptr->HandleEvents(Events);
}

void UpdateGenerator::Update()
{
	for (auto x : m_Children)
		x->Update();
}

void UpdateGenerator::Render()
{
	for (auto x : m_Children)
		x->Render();
}

void UpdateGenerator::AddChild(IUpdatable * Child)
{
	auto res = std::find_if(std::begin(m_Children), std::end(m_Children), [&](auto a) {return a == Child; });
	if (res == std::end(m_Children))
		m_Children.push_back(Child);
}

void UpdateGenerator::RemoveChild(IUpdatable * Child)
{
	auto res = std::find_if(std::begin(m_Children), std::end(m_Children), [&](auto a) {return a == Child; });
	if (res != std::end(m_Children))
		m_Children.erase(res);
}

UpdateGenerator::~UpdateGenerator()
{
	m_Children.clear();
}

IUpdatable::IUpdatable()
{
	AttachToUpdateGenerator();
}

IUpdatable::~IUpdatable()
{
}

void IUpdatable::AttachToUpdateGenerator()
{
	UpdateGenerator::GetInstance().AddChild(this);
}

void IUpdatable::RemoveFromUpdateGenerator()
{
	UpdateGenerator::GetInstance().RemoveChild(this);
}

DeltaTimer & DeltaTimer::GetInstance()
{
	static DeltaTimer instance;
	return instance;
}

void DeltaTimer::Update()
{
	UpdateDeltaTime();
}

void DeltaTimer::UpdateDeltaTime()
{
	m_Last = m_Now;
	m_Now = SDL_GetPerformanceCounter();

	auto dork = m_Now - m_Last;
	auto whatever = SDL_GetPerformanceFrequency();

	m_DeltaTime = static_cast<float>((m_Now - m_Last) * 1000 / static_cast<float>(SDL_GetPerformanceFrequency()));
}

float DeltaTimer::GetDeltaTime() 
{ 
	return GetInstance().m_DeltaTime;
}
