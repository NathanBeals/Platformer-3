#pragma once
//It's called game object, help me.

#include "Update.h"
#include "Images.h"
#include <vector>

class BoxCollider;

//Image + Colliders + Weight
class PhysicsObject
{
public:
	PhysicsObject() {};



private:
	std::vector<BoxCollider> m_Colliders = std::vector<BoxCollider>(); 
	float m_Weight = 0.0f;
	float m_XOffset, m_YOffset = 0;
	float m_XVelocity, m_YVelocity = 0;
};

class BoxCollider
{
	BoxCollider() {};
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
	
	
	
	*/
}
 