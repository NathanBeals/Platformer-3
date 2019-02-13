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
	void SetVelocity(float x, float y) { SetVelocity(Vector2f(x, y)); };
	
	//Forces?
	virtual void ApplyFriction();
	virtual void ApplyGravity();
	virtual void ApplyForces();

	void ApplyForce(float x, float y);

	void SetKinematic(bool Kinematic) { m_Kinematic = Kinematic; }
	void SetStatic(bool Static) { m_Static = Static; }

	void FloorVelocity()
	{
		if (abs(m_Velocity.x) < 0.1f) 
			m_Velocity.x = 0;
		if (abs(m_Velocity.y) < 0.1f) 
			m_Velocity.y = 0;
	}

private:
	PhysicsManager* m_PhysicsManager = nullptr;
	std::vector<SDL_Rect> m_Colliders = std::vector<SDL_Rect>();
	float m_Weight = 0.0f;
	bool m_Kinematic = false;
	bool m_Static = false;
	Vector2f* m_ParentOffset = nullptr;
	Vector2f m_Offset = Vector2f(0, 0);
	Vector2f m_Velocity = Vector2f(0, 0);
	
	//Resistance Forces
	float m_VGravity = 400;
	float m_AirFriction = 2;
};