#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTME_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem : public System
{
public:
	MovementSystem() {
		//requireComponent<TransformComponent>();
		//requireComponent<RigidBodyComponent>();
	}
	void updateMovement(double deltaTime) {
		// Loop all entities that the system is interested in
		for (auto entity : getSystemEntities()) {
			// Update entity position based on its velocity
			auto& transform = entity.getComponent<TransformComponent>();
			const auto rigidbody = entity.getComponent<RigidBodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;

			Logger::Log(
				"Entity id = " + 
				std::to_string(entity.getEntityId()) + 
				" position is now (" + 
				std::to_string(transform.position.x) + 
				", " + 
				std::to_string(transform.position.y) +
				")"
			);
		}
	}

private:

};

#endif // !MOVEMENTSYSTEM_H
