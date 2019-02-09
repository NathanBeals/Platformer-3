#pragma once

#include "Update.h"
#include "PhysicsVectors.h"

#include <vector>

/*
The PhysicsManagerContainer contains a list PhysicsManager, queryable by string name and created upon request.
The PhysicsManager contains a number of physics objects that can collide with eachother, and is capable of calcualting collisions bettween them on the global update.
The PhysicsObject contains a list of SDL_Rects which act as square colliders, weight, x/yoffsets, and x/yvelocities.
Essentially it should work like this, 
	An object that wishes to have collision will have a member of type PhysicsObject, and in its own update will query its draw offsets from the PhysicsObject.
	On creation an object should set up it's PhysicsObject's bounds to ensure it has collision. //TODO: add helper funtions to do this from file, from sprite, from graphic, etc

	When a new physics object is made it will try to add itself to a PhysicsManager by string name, The PhysicsManagerContainer will handle the request by creating a new PhysicsManager that can from then on
	be referenced by calling GetManger(std::string Name). The Object will then start recieving physics updates from the PhysicsManager and be able to collide with anything stored in that manager (and other managers if it adds itself to multiple).
	Every Update to a PhysicsObject will add velocities, change the x/yOffsets, and update the bounds by the new offsets.
*/

struct Vector2f;
class PhysicsObject;
class PhysicsManager;
struct PhysicsManagerNamedPair;

class PhysicsManagerContainer
{
public:
	PhysicsManagerContainer(PhysicsManagerContainer const&) = delete;
	void operator=(PhysicsManagerContainer const &) = delete;
	static PhysicsManagerContainer& GetInstance();

	PhysicsManager* GetManger(std::string const Name);

private:
	std::vector<PhysicsManagerNamedPair> m_Children = std::vector<PhysicsManagerNamedPair>();

	PhysicsManagerContainer() { };
	~PhysicsManagerContainer();
};

//List of Physics Objects by layer
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
	void ProcessCollision(PhysicsObject * A, PhysicsObject * B, SDL_Rect *aCol, SDL_Rect *bCol, SDL_Rect * Overlap);
	void ForceObjectOutOfWay(PhysicsObject *LighterObject, SDL_Rect *Collider, SDL_Rect *Overlap);

	bool CheckIntersection(PhysicsObject * A, PhysicsObject * B, SDL_Rect *ACollider, SDL_Rect *BCollider, SDL_Rect *Overlap);
};

//Colliders + Weight
class PhysicsObject
{
public:
	//Offsets will be controlled by the PhysicsObject
	PhysicsObject(std::string PhysicsManager, std::vector<SDL_Rect> Colliders, float Weight, Vector2f *Offset)
		: m_PhysicsManager(PhysicsManager)
		, m_Colliders(Colliders)
		, m_Weight(Weight)
		, m_ParentOffset(Offset)
	{
		SetOffset(*m_ParentOffset);
		PhysicsManagerContainer::GetInstance().GetManger(PhysicsManager)->AddChild(this);
	};

	std::vector<SDL_Rect> const *GetRects() const;;

	void Update();

	float GetWeight() const;
	Vector2f GetOffset() const;
	Vector2f GetVelocity() const;

	void SetWeight(float Weight);
	void SetOffset(Vector2f Offset); //Not only updates local offset, also updates parent offset
	void SetVelocity(Vector2f Velocity);

private:
	std::string m_PhysicsManager = "";
	std::vector<SDL_Rect> m_Colliders = std::vector<SDL_Rect>();
	float m_Weight = 0.0f;
	Vector2f* m_ParentOffset = nullptr;
	Vector2f m_Offset = Vector2f(0, 0);
	Vector2f m_Velocity = Vector2f(0, 0);
};

//Helpers to construct the collider lists needed by PhysicObjects
namespace Colliders
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