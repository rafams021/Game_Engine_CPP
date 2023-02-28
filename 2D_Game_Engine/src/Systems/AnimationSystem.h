#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include <SDL.h>

class AnimationSystem : public System {
public:
	AnimationSystem() {
		requireComponent<SpriteComponent>();
		requireComponent<AnimationComponent>();
	}

	void updateAnimation() {
		for (auto entity : getSystemEntities()) {
			auto& animation = entity.getComponent<AnimationComponent>();
			auto& sprite = entity.getComponent<SpriteComponent>();

			animation.currentFrame = ((SDL_GetTicks() - animation.startTime) * animation.frameSpeedRate / 1000) % animation.numFrames;
			sprite.srcRect.x = animation.currentFrame * sprite.width;
		}
	}
};

#endif // !ANIMATIONSYSTEM_H
