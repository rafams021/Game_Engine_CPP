#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

#include "../ECS/ECS.h";
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"
#include <SDL.h>

class RenderCollisionSystem : public System {
public:
	RenderCollisionSystem() {
		requireComponent<TransformComponent>();
		requireComponent<BoxColliderComponent>();
	}

	void updateRenderCollision(SDL_Renderer* renderer) {
		for (auto entity : getSystemEntities()) {
			const auto transform = entity.getComponent<TransformComponent>();
			const auto collider = entity.getComponent<BoxColliderComponent>();

			SDL_Rect colliderRect = {
				static_cast<int>(transform.position.x + collider.offset.x),
				static_cast<int>(transform.position.y + collider.offset.y),
				static_cast<int>(collider.width),
				static_cast<int>(collider.height)
			};
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &colliderRect);
		}
	}
};

#endif // !RENDERCOLLIDERSYSTEM_H

