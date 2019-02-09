#pragma once

#include <vector>
#include <SDL.h>

//Attaches self as listener to UpdateGenerator on construction
class IUpdatable
{
public:
	IUpdatable();
	~IUpdatable();

	virtual void Update() {};
	virtual void Render() {};

	void AttachToUpdateGenerator();
	void RemoveFromUpdateGenerator();
};

//Funnels Update/Render calls from the main loop into its children
//It's a singleton, the IUpdatables will latch onto it on their construction
class UpdateGenerator
{
public:
	UpdateGenerator(UpdateGenerator const&) = delete;
	void operator=(UpdateGenerator const &) = delete;
	static UpdateGenerator& GetInstance();

	void HandleEvents(std::vector<SDL_Event> * Events);
	void Update();
	void Render();

	void AddChild(IUpdatable *Child);
	void RemoveChild(IUpdatable *Child);

private:
	std::vector<IUpdatable*> m_Children = std::vector<IUpdatable*>();

	UpdateGenerator() { };
	~UpdateGenerator();
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

//Testing code, remove / comment out later
class UpdatableClassTest : public IUpdatable
{
public:
	UpdatableClassTest()
		: IUpdatable()
	{

	}

	void Update() override
	{
		auto dtim = DeltaTimer::GetDeltaTime();
		//if (dtim > 300.0 && dtim < 600)
		//	printf("it happened\n");
		printf("Delta Time: %f\n", DeltaTimer::GetDeltaTime());
		printf("Update\n");
	}
};