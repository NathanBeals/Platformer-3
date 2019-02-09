#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "Update.h"
#include "PhysicsVectors.h"
#include "PhysicsObjects.h"

//Helpers to construct the collider lists needed by PhysicObjects
namespace
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

namespace RenderImages
{
	class PNGImage : public IUpdatable
	{
	public:
		PNGImage(SDL_Renderer *Renderer, std::string FilePath);

		~PNGImage();

		//redundant with setoffset (but it does have the better name)
		void SetDrawPosition(int x = 0, int y = 0);

		void Render() override;
		void SetOffset(Vector2f Offset) { m_Offset = Offset; }

	private:
		SDL_Renderer *m_Renderer = nullptr;
		SDL_Surface *m_Surface = nullptr;
		SDL_Texture *m_Texture = nullptr;

		std::string m_FilePath;
		Vector2f m_Offset = Vector2f(0, 0);

		void LoadImage();
	};

	class SimpleImageObject : IUpdatable
	{
	public:
		SimpleImageObject(SDL_Renderer *Renderer, std::string ImageFilePath, PhysicsManager* PhysicsManager)
			: IUpdatable()
			, m_Image(Renderer, ImageFilePath)
			, m_Physics(PhysicsManager, CreateSimpleBoxCollider(0, 0, 50, 50), 10.0f, &m_Offset)
		{

		}

		//TODO: think about interactions
		void Update() override
		{
			////TODO: Friction
			//auto v = m_Physics.GetVelocity();
			//v.x /= 2;
			//v.y /= 2;
			//m_Physics.SetVelocity(v);

			m_Image.SetOffset(m_Offset);
		}

	public:
		PNGImage m_Image;
		Vector2f m_Offset = Vector2f(50, 50);
		PhysicsObject m_Physics;
	};
}