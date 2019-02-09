#pragma once

#include "Update.h"
#include <vector>


struct Vector2f;
class PhysicsObject;
class PhysicsManager;

struct Vector2f
{
	Vector2f(float X, float Y) : x(X), y(Y) {};
	float x, y = 0;

	float Magnitude()
	{
		return std::sqrt(std::abs(x) + std::abs(y));
	}

	Vector2f operator*(float Magnitude)
	{
		return Vector2f(x * Magnitude, y * Magnitude);
	}

	Vector2f operator-(Vector2f vec)
	{
		return Vector2f(x - vec.x, y - vec.y);
	}

	bool operator ==(Vector2f vec)
	{
		return x == vec.x && y == vec.y;
	}
};

//Not a singleton like update, I'm thinking of having seperate layers of collidable objects
class PhysicsManager : IUpdatable
{
	PhysicsManager();

	void Update() override;

	void AddChild(PhysicsObject * Child);
	void RemoveChild(PhysicsObject * Child);

private:
	std::vector<PhysicsObject*> m_Children = std::vector<PhysicsObject*>();

	//Sees if any physics object collides with any other physics obejct managed by this container
//Applies forced movement (to stop overlapping), new velocities, and tells the colliders to update their offsets...
//TODO: think more on these comments
	//Current method won't handle multiple collisions gracefully, consider
	void ProcessCollisions();
	void ProcessCollision(PhysicsObject * A, PhysicsObject * B, SDL_Rect *aCol, SDL_Rect *bCol, SDL_Rect * Overlap);
	void ForceObjectOutOfWay(PhysicsObject *LighterObject, SDL_Rect *Collider, SDL_Rect *Overlap);

	bool CheckIntersection(PhysicsObject * A, PhysicsObject * B, SDL_Rect *ACollider, SDL_Rect *BCollider, SDL_Rect *Overlap);
};

//Image + Colliders + Weight
class PhysicsObject
{
public:
	PhysicsObject()
	{
		//Set Offsets after everything else
		//TO be filled
	}; //run setoffsets and such

	//TODO: consider just storing them as SDL_Rects in the first place
	//Indstead of handling the offsets in update, just update them on request 
	//Now hold on one godamn minute
	//This does nothing
	//Offsets are stored in the to be parent of this object (like the images), why am I doing this at all?
	//rrrrrrrrrrrr
	std::vector<SDL_Rect> const *GetRects() const;;

	void Update();

	float GetWeight() const;
	Vector2f GetOffset() const;
	Vector2f GetVelocity() const;

	void SetWeight(float Weight);
	void SetOffset(Vector2f Offset);
	void SetVelocity(Vector2f Velocity);

private:
	PhysicsManager *m_PManager = nullptr;
	std::vector<SDL_Rect> m_Colliders = std::vector<SDL_Rect>();
	float m_Weight = 0.0f;
	Vector2f m_Offset = Vector2f(0, 0);
	Vector2f m_Velocity = Vector2f(0, 0);
};

namespace Collisions
{
	//TODO: Collisions

	/*
	My Idea is this, objects are made up of a number of rect colliders,
	these non-overlapping colliders are stored in a vector and associated with an image,
	they are to be stored in xml simmilarily to how spritesheets are,
	it's likely I'll have to rework spritesheets to accomodate them,
	or I could auto generate the colliders based on pixel fills or something.
	When a collision occurs, the lower weight object is moved out of the colliding area,
	and forces are applied depending on the relative weights and velocities.

	In the case of superheavy objects like terrain, if the force applied is trivial to the weight of an object it will be rounded out to having no effect.
	In the case of objects of simmilar weight, the lighter object will be pushed out an the relative velocities of the two bodies will equalize based on the imparted force.
	//If one 150lb person slams into another stationary 150lb man at 5m/s their resultant velocities will be 300lbs at 2.5m/s, that's my thoery atleast.
	I'm still thinking about angles
	If we simplify two colliding rectanges as two lines parallel to the rectangles longest axis we have two lines, if we shorten their ends by half the shortest axis, we will have two short lines representing the cores of the rectangles.
	If we get the extents of the complex rectangles and where they intercept the second rectangle (say the bot left corner and a point halfway throught the bot) we can then find the shortest distance from the center to the rectangle cores.
	Once we have the angle the lines make from their respective cores, we can apply that velocity to the whole object.

	consider SDL_IntersectRect

	*/
}
