#pragma once

#include <cmath>

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