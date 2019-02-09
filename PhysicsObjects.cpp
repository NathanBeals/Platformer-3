#include "PhysicsObjects.h"

#include <math.h>
#include <algorithm>

PhysicsManager::PhysicsManager()
	: IUpdatable()
{

}

void PhysicsManager::Update()
{
	ProcessCollisions();
}

void PhysicsManager::AddChild(PhysicsObject * Child)
{
	auto res = std::find_if(std::begin(m_Children), std::end(m_Children), [&](auto a) {return a == Child; });
	if (res == std::end(m_Children))
		m_Children.push_back(Child);
}

void PhysicsManager::RemoveChild(PhysicsObject * Child)
{
	auto res = std::find_if(std::begin(m_Children), std::end(m_Children), [&](auto a) {return a == Child; });
	if (res != std::end(m_Children))
		m_Children.erase(res);
}

//Sees if any physics object collides with any other physics obejct managed by this container
//Applies forced movement (to stop overlapping), new velocities, and tells the colliders to update their offsets...
//TODO: think more on these comments
//Current method won't handle multiple collisions gracefully, consider

void PhysicsManager::ProcessCollisions()
{
	for (auto a : m_Children)
	{
		for (auto b : m_Children)
		{
			if (&a != &b) continue;//So objects don't try to collide with themselves into eternity

			SDL_Rect aCol, bCol, overlap;
			if (CheckIntersection(a, b, &aCol, &bCol, &overlap))
				ProcessCollision(a, b, &aCol, &bCol, &overlap);
		}
	}
}

//TODO: look up pointer to const syntax, I haven't been consting nearly as much as I should be. I should make some kind of big jira ticket or something for this if I ever make a ticket system (ADD CONSTYNESS)

bool PhysicsManager::CheckIntersection(PhysicsObject * A, PhysicsObject * B, SDL_Rect *ACollider, SDL_Rect *BCollider, SDL_Rect *Overlap)
{
	for (auto a : *A->GetRects())
		for (auto b : *B->GetRects())
			if (SDL_IntersectRect(&a, &b, Overlap))
			{
				ACollider = &a;
				BCollider = &b;
				return true;
			}

	return false;
}

void PhysicsManager::ProcessCollision(PhysicsObject * A, PhysicsObject * B, SDL_Rect *aCol, SDL_Rect *bCol, SDL_Rect * Overlap)
{
	//New Velocities
	auto resXVector = (A->GetWeight() * A->GetVelocity().x + B->GetWeight() * B->GetVelocity().x) / (A->GetWeight() + B->GetWeight());
	auto resYVector = (A->GetWeight() * A->GetVelocity().x + B->GetWeight() * B->GetVelocity().x) / (A->GetWeight() + B->GetWeight());
	A->SetVelocity(Vector2f(resXVector, resYVector));
	B->SetVelocity(Vector2f(resXVector, resYVector));

	//Move out of way
	auto lightObj = A->GetWeight() < B->GetWeight() ? A : B;
	if (A->GetWeight() < B->GetWeight())
		ForceObjectOutOfWay(A, aCol, Overlap);
	else
		ForceObjectOutOfWay(B, bCol, Overlap);
}


//Temporarily Modify velocity to push away rect centers until object is free
//TODO: work here again, this is using point not core so long rectangular colliders will push harder towards the larger axis
void PhysicsManager::ForceObjectOutOfWay(PhysicsObject * LighterObject, SDL_Rect *Collider, SDL_Rect * Overlap)
{
	Vector2f objectCenter = Vector2f(static_cast<float>(Collider->x + Collider->w / 2), static_cast<float>(Collider->y + Collider->h / 2));
	Vector2f overlapCenter = Vector2f(static_cast<float>(Overlap->x + Overlap->w / 2), static_cast<float>(Overlap->y + Overlap->h / 2));
	Vector2f escVector = objectCenter - overlapCenter;

	if (objectCenter == overlapCenter) return; //Wait for resolution by vector movement

	Vector2f oVector = LighterObject->GetVelocity();
	while (SDL_HasIntersection(Collider, Overlap))
	{
		LighterObject->SetVelocity(escVector);
		LighterObject->Update(); //Move with velocity, this should update the colliders and eventualy move it out of the way (or crash the game, prolly crash the game)
	}

	//Restore original velocity
	LighterObject->SetVelocity(oVector);
}

std::vector<SDL_Rect> const * PhysicsObject::GetRects() const
{
	return &m_Colliders;
}

void PhysicsObject::Update()
{
	SetOffset(Vector2f(m_Offset.x + m_Velocity.x, m_Offset.y + m_Velocity.y));
}

float PhysicsObject::GetWeight() const { return m_Weight; }

Vector2f PhysicsObject::GetOffset() const { return m_Offset; }

Vector2f PhysicsObject::GetVelocity() const { return m_Velocity; }

void PhysicsObject::SetWeight(float Weight) { m_Weight = Weight; }

void PhysicsObject::SetOffset(Vector2f Offset)
{
	m_Offset = Offset;
	for (auto x : m_Colliders)
	{
		x.x = static_cast<int>(std::round(m_Offset.x));
		x.y = static_cast<int>(std::round(m_Offset.y));
	}
}

void PhysicsObject::SetVelocity(Vector2f Velocity) { m_Velocity = Velocity; }
