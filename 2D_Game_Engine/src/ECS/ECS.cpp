#include "ECS.h"
#include "../Logger/Logger.h"
#include <algorithm>

int IComponent::nextId = 0;

int Entity::getEntityId() const {
	return id;
}


void System::addEntityToSystem(Entity entity) {
	entities.push_back(entity);
}


void System::removeEntityFromSystem(Entity entity) {
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) { //Lambda Function implementation
		return entity == other;
		}), entities.end());
}


std::vector<Entity> System::getSystemEntities() const {
	return entities;
}


const Signature& System::getComponentSignature() const {
	return componentSignature;
}


Entity Registry::createEntity() {
	int entityId;

	entityId = numEntities++;

	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	if (entityId >= static_cast<int>(entityComponentSignatures.size())) {
		entityComponentSignatures.resize(entityId + 1);
	}

	Logger::Log("Entity created with id = " + std::to_string(entityId));

	return entity;
}


void Registry::addEntityToSystems(Entity entity) {
	const auto entityId = entity.getEntityId();

	const auto& entityComponentSignature = entityComponentSignatures[entityId];

	for (auto& system : systems) {
		const auto& systemComponentSignature = system.second->getComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested) {
			system.second->addEntityToSystem(entity);
		}
	}
}


void Registry::updateRegistry() {
	// Add the entities that are waiting to be created to the active Systems
	for (auto entity : entitiesToBeAdded) {
		addEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	// TODO
}