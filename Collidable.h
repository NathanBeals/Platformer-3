#pragma once

class ICollidable
{
public:
	ICollidable() {};

protected:
	void virtual HandleCollision(ICollidable Collider)
	{
		//Handle extra collision data with Collider
	}
};7