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

//HACK: Current method won't handle multiple collisions gracefully, consider
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

namespace
{
	Vector2f GetCenter(SDL_Rect const *Rect)
	{
		return Vector2f(Rect->x + Rect->w / 2.0f, Rect->y + Rect->h / 2.0f);
	}
}

//HACK: just, refactor
//HACK: this formula is not correct
//If I make contact with perpendicular velocity, (to a superheavy object), my perpendicular velocity is completely negated
void PhysicsManager::ProcessCollision(PhysicsObject * A, PhysicsObject * B)
{
	auto rects = GetIntersectingRects(A, B);
	auto *lighter = A->GetWeight() < B->GetWeight() ? A : B;
	auto *heavier = A->GetWeight() < B->GetWeight() ? B : A;

	auto lV = lighter->GetVelocity();
	auto lAngle = atan2f(lV.x, lV.y);
	auto lCenter = GetCenter(lighter == A ? &rects.a : &rects.b);
	auto ovCenter = GetCenter(&rects.overlap);
	float angleToOverlapCenter = std::atan2f(lCenter.y - ovCenter.y, ovCenter.x - lCenter.x);

	float friction = (1 / 5) * DeltaTimer::GetDeltaTime(); //TODO: better number
	auto angle = .80f;  //TODO: pick actual angle
	auto rV = lV;
	if (std::sin(angleToOverlapCenter) < -angle) //UP
		if (rV.y > 0) rV.y = -rV.y * friction;

	if (std::sin(angleToOverlapCenter) > angle) //Down
		if (rV.y > 0) rV.y = -rV.y * friction;

	if (std::cos(angleToOverlapCenter) < -angle) //Left
		if (rV.x < 0) rV.x = -rV.x * friction;

	if (std::cos(angleToOverlapCenter) > angle) //Right
		if (rV.x > 0) rV.x = -rV.x * friction;
	lighter->SetVelocity(rV);

	//Move the lighter object out of the way (ignoring actual physical interactions)
	ForceObjectOutOfWay(A, B);
}

//Temporarily Modify velocity to push away rect centers until object is free
//HACK: this code will just push away from the center until a good position is reached, that wont look right for long or tall rects 
void PhysicsManager::ForceObjectOutOfWay(PhysicsObject *A, PhysicsObject *B)
{
	auto rects = GetIntersectingRects(A, B);
	auto overlap = rects.overlap;
	auto *lighter = A->GetWeight() < B->GetWeight() ? A : B;
	auto *heavier = A->GetWeight() < B->GetWeight() ? B : A;

	//Move out of the overlapping rect
	auto lCenter = GetCenter(lighter == A ? &rects.a : &rects.b);
	auto ovCenter = GetCenter(&rects.overlap);

	//Just move the lighter object out of the intersection box
	auto newOffset = lighter->GetOffset();
	if (overlap.w < overlap.h)
	{
		if (ovCenter.x < lCenter.x)
			newOffset.x += overlap.w / 2;
		else if (ovCenter.x > lCenter.x)
			newOffset.x -= overlap.w / 2;
	}
	else
	{
		if (ovCenter.y < lCenter.y)
			newOffset.y += overlap.h / 2;
		else if (ovCenter.y > lCenter.y)
			newOffset.y -= overlap.h / 2;
	}
	lighter->SetOffset(newOffset);
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
	m_Velocity.x /= 1 + (m_AirFriction * DeltaTimer::GetDeltaTime() / 1000); //TODO: member variable
	m_Velocity.y /= 1 + (m_AirFriction * DeltaTimer::GetDeltaTime() / 1000);
}

void PhysicsObject::ApplyGravity()
{
	//Ignore overly large inputs
	if ((DeltaTimer::GetDeltaTime() / 1000) > 20) return;

	//Add Gravitational force, more for heavy things? //HACK:
	ApplyForce(0, m_Weight * m_VGravity * (DeltaTimer::GetDeltaTime() / 1000));
}

void PhysicsObject::ApplyForces()
{
	if (!m_Kinematic)
	{
		ApplyGravity();
		ApplyFriction();
		printf("Velocity: %f,%f-\n", GetVelocity().x, GetVelocity().y);
	}

	if (m_Static)
		SetVelocity(0, 0);

	FloorVelocity();
}

void PhysicsObject::ApplyForce(float x, float y)
{
	m_Velocity += Vector2f(x / m_Weight, y / m_Weight);
}
