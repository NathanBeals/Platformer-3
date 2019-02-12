#pragma once

#include "Update.h"
#include "PhysicsVectors.h"

#include <vector>
#include <algorithm>

/*
The PhysicsManager contains a number of physics objects that can collide with eachother, and is capable of calcualting collisions bettween them on the global update.
The PhysicsObject contains a list of SDL_Rects which act as square colliders, weight, x/yoffsets, and x/yvelocities.
Essentially it should work like this, 
	An object that wishes to have collision will have a member of type PhysicsObject, and in its own update will query its draw offsets from the PhysicsObject.
	On creation an object should set up it's PhysicsObject's bounds to ensure it has collision. //TODO: add helper funtions to do this from file, from sprite, from graphic, etc

	A Physics Object will have a manager passed to it and each manager will handdle collision between the objects it owns 
*/

struct Vector2f;
class PhysicsObject;
class PhysicsManager;

//Object interaction

//Handles interactions between physics objects
class PhysicsManager : IUpdatable
{
public:
	PhysicsManager();

	void Update() override;

	void AddChild(PhysicsObject * Child);
	void RemoveChild(PhysicsObject * Child);

private:
	std::vector<PhysicsObject*> m_Children = std::vector<PhysicsObject*>();

	void ProcessCollisions();
	void ProcessCollision(PhysicsObject * A, PhysicsObject * B);
	void ForceObjectOutOfWay(PhysicsObject * A, PhysicsObject* B);

	bool CheckIntersection(PhysicsObject * A, PhysicsObject * B, SDL_Rect *Overlap);
};

//Colliders + Weight
class PhysicsObject
{
public:
	//Offsets will be controlled by the PhysicsObject
	PhysicsObject(PhysicsManager* PhysicsManager, std::vector<SDL_Rect> Colliders, float Weight, Vector2f *Offset);
	~PhysicsObject();

	std::vector<SDL_Rect> const GetRects() const;

	void Update();

	float GetWeight() const;
	Vector2f GetOffset() const;
	Vector2f GetVelocity() const;

	void SetWeight(float Weight);
	void SetOffset(Vector2f Offset);
	void SetVelocity(Vector2f Velocity);
	
	virtual void ApplyFriction()
	{
		auto v = GetVelocity();
		ApplyVelocityFloor();

		//Very simple air resistance, essentially just
		auto adjFriction = (m_VAir / m_Weight) * DeltaTimer::GetDeltaTime() / 1000;

		if (abs(v.x) - abs(adjFriction.x) <= 0) 
			v.x = 0;
		else
			!std::signbit(v.x) ? v.x -= adjFriction.x : v.x += adjFriction.x; //If positive

		if (abs(v.y) - abs(adjFriction.y) <= 0) 
			v.y = 0;
		else
			!std::signbit(v.y) ? v.y -= adjFriction.y : v.y += adjFriction.y; //If positive

		m_Velocity = v;
	}

	void ApplyGravity()
	{
		//Ignore overly large inputs
		if ((DeltaTimer::GetDeltaTime() / 1000) > 100) return;
		auto grav = m_VGravity * (DeltaTimer::GetDeltaTime() / 1000);
		m_Velocity += grav;
	}

	virtual void ApplyForces()
	{
		ApplyVelocityFloor();
		ClampVelocity();
		if (m_Kinematic) return;

		ApplyGravity();
		ApplyFriction();
		ClampVelocity();
	}

	void SetKinematic(bool Kinematic)
	{
		m_Kinematic = Kinematic;
	}

	//If a velocity is increadibly low the force imparted by a collision was insignifigant
	void ApplyVelocityFloor()
	{
		if (abs(m_Velocity.x) < m_VMinimumn.x) m_Velocity.x = 0;
		if (abs(m_Velocity.y) < m_VMinimumn.y) m_Velocity.y = 0;
	}

	void ClampVelocity()
	{ 
		if (abs(m_Velocity.x) > m_MaxVelocity.x)
			!signbit(m_Velocity.x) ? m_Velocity.x = m_MaxVelocity.x : m_Velocity.x = -m_MaxVelocity.x;
		if (abs(m_Velocity.y) > m_MaxVelocity.y)
			!signbit(m_Velocity.y) ? m_Velocity.y = m_MaxVelocity.y : m_Velocity.y = -m_MaxVelocity.y;
	}

private:
	PhysicsManager* m_PhysicsManager = nullptr;
	std::vector<SDL_Rect> m_Colliders = std::vector<SDL_Rect>();
	float m_Weight = 0.0f;
	bool m_Kinematic = false;
	Vector2f* m_ParentOffset = nullptr;
	Vector2f m_Offset = Vector2f(0, 0);
	Vector2f m_Velocity = Vector2f(0, 0);
	
	//Resistance Forces
	Vector2f m_VMinimumn = Vector2f(10, 10);
	Vector2f m_VAir = Vector2f(1000, 1000); //Reconsider air friction, always positive
	Vector2f m_VGravity = Vector2f(0, 30.0f);
	Vector2f m_MaxVelocity = Vector2f(200, 200);
};