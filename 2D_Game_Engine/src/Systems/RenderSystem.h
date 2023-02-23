#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL.h>

class RenderSystem : public System
{
public:
	RenderSystem() {
		requireComponent<TransformComponent>();
		requireComponent<SpriteComponent>();
	}
	void updateRender(SDL_Renderer* renderer) {
		// Loop all entities that the system is interested in
		for (auto entity : getSystemEntities()) {
			// Update entity position based on its velocity
			const auto transform = entity.getComponent<TransformComponent>();
			const auto sprite = entity.getComponent<SpriteComponent>();

			SDL_Rect objRect = {
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				sprite.width,
				sprite.height
			};
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(renderer, &objRect);
		}
	}

private:

};

#endif // !RENDERSYSTEM_H