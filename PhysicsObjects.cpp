#include "PhysicsObjects.h"

#include <math.h>
#include <algorithm>

namespace
{
	struct IntesectingRects
	{
		bool bValid = false;
		SDL_Rect a;
		SDL_Rect b;
		SDL_Rect overlap;
	};

	IntesectingRects GetIntersectingRects(PhysicsObject * A, PhysicsObject * B)
	{
		auto result = IntesectingRects();

		for (auto a : A->GetRects())
			for (auto b : B->GetRects())
				if (SDL_IntersectRect(&a, &b, &result.overlap))
				{
					result.a = a;
					result.b = b;
					result.bValid = true;
				}

		return result;
	}
}

PhysicsManager::PhysicsManager()
	: IUpdatable()
{

}

void PhysicsManager::Update()
{
	for (auto x : m_Children)
		x->Update();

	ProcessCollisions();
	//ProcessGraivity();
	//ProcessFriction();
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

//TODO: Current method won't handle multiple collisions gracefully, consider
void PhysicsManager::ProcessCollisions()
{
	for (auto a : m_Children)
	{
		for (auto b : m_Children)
		{
			if (a == b) continue;

			IntesectingRects rects;
			rects = GetIntersectingRects(a, b);
			if (rects.bValid)
				ProcessCollision(a, b);
		}
	}
}

bool PhysicsManager::CheckIntersection(PhysicsObject * A, PhysicsObject * B, SDL_Rect *Overlap)
{
	for (auto a : A->GetRects())
		for (auto b : B->GetRects())
			if (SDL_IntersectRect(&a, &b, Overlap))
				return true;

	return false;
}

//HACK: this formula is not correct
//If I make contact with perpendicular velocity, (to a superheavy object), my perpendicular velocity is completely negated
void PhysicsManager::ProcessCollision(PhysicsObject * A, PhysicsObject * B)
{
	//New Velocities
	auto resXVector = (A->GetWeight() * A->GetVelocity().x + B->GetWeight() * B->GetVelocity().x) / (A->GetWeight() + B->GetWeight());
	auto resYVector = (A->GetWeight() * A->GetVelocity().y + B->GetWeight() * B->GetVelocity().y) / (A->GetWeight() + B->GetWeight());
	
	//Instead of set velocity try adding force
	A->SetVelocity(Vector2f(resXVector, resYVector));
	B->SetVelocity(Vector2f(resXVector, resYVector));
	//A->SetVelocity(Vector2f(A->GetVelocity().x, resYVector));
	//B->SetVelocity(Vector2f(B->GetVelocity().x, resYVector));

	//Move out of way
	ForceObjectOutOfWay(A, B);
}


//Temporarily Modify velocity to push away rect centers until object is free
//HACK: this code will just push away from the center until a good position is reached, that wont look right for long or tall rects 
void PhysicsManager::ForceObjectOutOfWay(PhysicsObject *A, PhysicsObject *B)
{
	auto rects = GetIntersectingRects(A, B);
	while (rects.bValid)
	{
		auto lighter = A->GetWeight() < B->GetWeight() ? A : B;
		auto pushee = A->GetWeight() < B->GetWeight() ? rects.a : rects.b;
		auto pusher = rects.overlap;

		Vector2f objectCenter = Vector2f(static_cast<float>(pushee.x + pushee.w / 2), static_cast<float>(pushee.y + pushee.h / 2));
		Vector2f overlapCenter = Vector2f(static_cast<float>(pusher.x + pusher.w / 2), static_cast<float>(pusher.y + pusher.h / 2));
		Vector2f escVector = objectCenter - overlapCenter;

		if (objectCenter == overlapCenter) return; //Wait for resolution by vector movement

		Vector2f oVector = lighter->GetVelocity();
		lighter->SetVelocity(escVector);
		lighter->Update(); //Move with velocity, this should update the colliders and eventualy move it out of the way (or crash the game, prolly crash the game)
		lighter->SetVelocity(oVector);

		rects = GetIntersectingRects(A, B);
	}
}


//Offsets will be controlled by the PhysicsObject

PhysicsObject::PhysicsObject(PhysicsManager * PhysicsManager, std::vector<SDL_Rect> Colliders, float Weight, Vector2f * Offset)
	: m_PhysicsManager(PhysicsManager)
	, m_Colliders(Colliders)
	, m_Weight(Weight)
	, m_ParentOffset(Offset)
{
	SetOffset(*m_ParentOffset);
	if (PhysicsManager) PhysicsManager->AddChild(this);
}

PhysicsObject::~PhysicsObject()
{
	m_PhysicsManager = nullptr;
}

std::vector<SDL_Rect> const PhysicsObject::GetRects() const
{
	return m_Colliders;
}

void PhysicsObject::Update()
{
	ApplyForces();
	SetOffset(Vector2f(m_Offset.x + m_Velocity.x * DeltaTimer::GetDeltaTime() / 1000, m_Offset.y + m_Velocity.y * DeltaTimer::GetDeltaTime() / 1000));
}

float PhysicsObject::GetWeight() const { return m_Weight; }

Vector2f PhysicsObject::GetOffset() const { return m_Offset; }

Vector2f PhysicsObject::GetVelocity() const { return m_Velocity; }

void PhysicsObject::SetWeight(float Weight) { m_Weight = Weight; }

void PhysicsObject::SetOffset(Vector2f Offset)
{
	m_Offset = Offset;
	for (auto &x : m_Colliders)
	{
		x.x = static_cast<int>(std::round(m_Offset.x));
		x.y = static_cast<int>(std::round(m_Offset.y));
	}

	if (m_ParentOffset != nullptr)
		*m_ParentOffset = m_Offset;
}

void PhysicsObject::SetVelocity(Vector2f Velocity) { m_Velocity = Velocity; }

//TODO: I've given up for now, just use the super simple one
void PhysicsObject::ApplyFriction()
{
	m_Velocity.x /= 1 + (.1 * DeltaTimer::GetDeltaTime() / 1000);
	m_Velocity.y /= 1 + (.1 * DeltaTimer::GetDeltaTime() / 1000);
}

void PhysicsObject::ApplyGravity()
{
	//Ignore overly large inputs
	if ((DeltaTimer::GetDeltaTime() / 1000) > 1) return;

	//Add Gravitational force, more for heavy things? //HACK:
	ApplyForce(0, m_Weight * m_VGravity * (DeltaTimer::GetDeltaTime() / 1000));
}

void PhysicsObject::ApplyForces()
{
	if (!m_Kinematic)
	{
		ApplyGravity();
		ApplyFriction();
	}

	if (m_Static)
		SetVelocity(0, 0);

	FloorVelocity();
}

void PhysicsObject::ApplyForce(float x, float y)
{
	m_Velocity += Vector2f(x / m_Weight, y / m_Weight);
}
