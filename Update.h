#pragma once
#include <vector>
#include <SDL.h>
//TODO: consider constness
//TODO: Update file name to Updater/UpdateGenerator/UpdateHandler/etc
class IUpdatable;

//Handles passing updates down the chain to any class implementing IUpdatable
//IUpdatables will attach themselves to the single Updater instance and handle updates by overriding their Update/Render methods
class Updater
{
public:
	Updater(Updater const&) = delete;
	void operator=(Updater const &) = delete;
	static Updater& GetInstance();

	void HandleEvents(std::vector<SDL_Event> * Events);
	void Update();
	void Render();

	void AddChild(IUpdatable *Child);
	void RemoveChild(IUpdatable *Child);

private:
	std::vector<IUpdatable*> m_Children = std::vector<IUpdatable*>();

	Updater() { };
	~Updater();
};

//Classes that wish to recieve updates/render calls will derive from this class and override the calls they wish to recieve
//The class will attach istelf to the static instance of Updater and get calls through it
class IUpdatable
{
public:
	IUpdatable();
	~IUpdatable();

	virtual void Update() {};
	virtual void Render() {};

	void AttachToUpdater();
	void RemoveFromUpdater();
};

//Once the instance is created (through a call to GetInstance()) it will add itself to Updaters children and start calcualting deltatimes for its own updates and,
//from then on anyone can query DeltaTimer::GetDeltaTime() to get how long it has been since the last update call (to the DeltaTimer instance)
class DeltaTimer : IUpdatable
{
public:
	DeltaTimer(DeltaTimer const&) = delete;
	void operator=(DeltaTimer const &) = delete;

	static DeltaTimer& GetInstance();

	void Update() override;
	void UpdateDeltaTime();
	static double GetDeltaTime();

private:
	uint64_t m_Now = 0;
	uint64_t m_Last = 0;
	double m_DeltaTime = 0.0;

	DeltaTimer() {};
	~DeltaTimer() {};
};

//Testing code, remove / comment out later
class TestUpdater : public IUpdatable
{
public:
	TestUpdater()
		: IUpdatable()
	{

	}

	void Update() override
	{
		auto dtim = DeltaTimer::GetDeltaTime();
		if (dtim > 300.0 && dtim < 600)
			printf("it happened\n");
		printf("Delta Time: %f\n", DeltaTimer::GetDeltaTime());
		printf("Update\n");
	}
};

class IEventHandler : public IUpdatable
{
public:
	IEventHandler()
		: IUpdatable()
	{

	}

	virtual void HandleEvents(std::vector<SDL_Event> * Events) 
	{
		for (auto x : *Events)
			HandleEvent(&x);
	};
	virtual void HandleEvent(SDL_Event * Event) {};
};