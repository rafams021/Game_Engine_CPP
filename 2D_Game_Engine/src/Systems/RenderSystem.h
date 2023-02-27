#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetManager/AssetManager.h"
#include <SDL.h>
#include <vector>
#include <algorithm>

class RenderSystem : public System
{
public:
	RenderSystem() {
		requireComponent<TransformComponent>();
		requireComponent<SpriteComponent>();
	}
	void updateRender(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetManager) {
		// Create a vector of Sprite and Transform Component of all entities
		struct RenderableEntity{
			TransformComponent transformComponent;
			SpriteComponent spriteComponent;
		};
		std::vector<RenderableEntity> renderableEntities;
		for (auto entity : getSystemEntities()) {
			RenderableEntity renderableEntity;
			renderableEntity.spriteComponent = entity.getComponent<SpriteComponent>();
			renderableEntity.transformComponent = entity.getComponent<TransformComponent>();
			renderableEntities.emplace_back(renderableEntity);
		}

		// Sort the vector by the z-index value
		std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
			return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
			});

		// Loop all entities that the system is interested in
		for (auto entity : renderableEntities) {
			// Update entity position based on its velocity
			const auto transform = entity.transformComponent;
			const auto sprite = entity.spriteComponent;

			// Set the source rectangle of our original sprite texture
			SDL_Rect srcRect = sprite.srcRect;

			// Set the destination rectangle with the x,y position to be rendered
			SDL_Rect dstRect = {
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				static_cast<int>(sprite.width * transform.scale.x),
				static_cast<int>(sprite.height * transform.scale.y)
			};

			SDL_RenderCopyEx(
				renderer,
				assetManager->getTexture(sprite.assetId),
				&srcRect,
				&dstRect,
				transform.rotation,
				NULL,
				SDL_FLIP_NONE
			);
		}
	}

private:

};

#endif // !RENDERSYSTEM_H