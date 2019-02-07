#include "Update.h"
#include <algorithm>
#include <SDL.h>

Updater & Updater::GetInstance()
{
	static Updater instance;
	return instance;
}

void Updater::HandleEvents(std::vector<SDL_Event>* Events)
{
	for (auto x : m_Children)
		if (auto ptr = dynamic_cast<IEventHandler*>(&*x))
			ptr->HandleEvents(Events);
}

void Updater::Update()
{
	for (auto x : m_Children)
		x->Update();
}

void Updater::Render() //TODO: Also pass the renderer maybe
{
	for (auto x : m_Children)
		x->Render();
}

void Updater::AddChild(IUpdatable * Child)
{
	auto res = std::find_if(std::begin(m_Children), std::end(m_Children), [&](auto a) {return a == Child; });
	if (res == std::end(m_Children))
		m_Children.push_back(Child);
}

void Updater::RemoveChild(IUpdatable * Child)
{
	auto res = std::find_if(std::begin(m_Children), std::end(m_Children), [&](auto a) {return a == Child; });
	if (res != std::end(m_Children))
		m_Children.erase(res);
}

Updater::~Updater()
{
	m_Children.clear();
}

IUpdatable::IUpdatable()
{
	AttachToUpdater();
}

IUpdatable::~IUpdatable()
{
}

void IUpdatable::AttachToUpdater()
{
	Updater::GetInstance().AddChild(this);
}

void IUpdatable::RemoveFromUpdater()
{
	Updater::GetInstance().RemoveChild(this);
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

	m_DeltaTime = (double)((m_Now - m_Last) * 1000 / (double)SDL_GetPerformanceFrequency());
}

double DeltaTimer::GetDeltaTime() 
{ 
	return GetInstance().m_DeltaTime;
}
