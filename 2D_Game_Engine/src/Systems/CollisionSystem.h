#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

class CollisionSystem : public System {
public:
	CollisionSystem() {
		requireComponent<TransformComponent>();
		requireComponent<BoxColliderComponent>();
	}

	void updateCollision() {
		auto entities = getSystemEntities();

		// Loop all the entities that the system is interested in
		for (auto i = entities.begin(); i != entities.end(); i++) {
			Entity entityA = *i;
			auto transformA = entityA.getComponent<TransformComponent>();
			auto colliderA = entityA.getComponent<BoxColliderComponent>();

			// Loop all the entites that still need to be checked
			for (auto j = i; j != entities.end(); j++) {
				Entity entityB = *j;

				// Bypass if we are in the same entity
				if (entityA == entityB) continue;

				auto transformB = entityB.getComponent<TransformComponent>();
				auto colliderB = entityB.getComponent<BoxColliderComponent>();

				// Perform AABB collision checj between entities A and B
				bool collisionHappened = checkAABBCollision(
					transformA.position.x + colliderA.offset.x,
					transformA.position.y + colliderB.offset.y,
					colliderA.width,
					colliderA.height,
					transformB.position.x + colliderB.offset.x,
					transformB.position.y + colliderB.offset.y,
					colliderB.width,
					colliderB.height
				);

				if (collisionHappened) {
					Logger::Log("Entity " + std::to_string(entityA.getEntityId()) + " is colliding with entity " + std::to_string(entityB.getEntityId()));

					entityA.deleteEntity();
					entityB.deleteEntity();
				}
			}
		}
	}

	bool checkAABBCollision(double entityA_X, double entityA_Y, double entityA_W, double entityA_H, double entityB_X, double entityB_Y, double entityB_W, double entityB_H) {
		return (
			entityA_X < entityB_X + entityB_W &&
			entityA_X + entityA_W > entityB_X &&
			entityA_Y < entityB_Y + entityB_H &&
			entityA_Y + entityA_H > entityB_Y
			);
	}
};

#endif // !COLLISIONSYSTEM_H

