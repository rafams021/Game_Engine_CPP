#include "ECS.h"
#include "../Logger/Logger.h"
#include <algorithm>

int IComponent::nextId = 0;

int Entity::getEntityId() const {
	return id;
}


void Entity::deleteEntity() {
	registry->killEntity(*this);
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

	if(freeIDs.empty()){ 
		// If there are no free IDs waiting to be reused
		entityId = numEntities++; 
		if (entityId >= static_cast<int>(entityComponentSignatures.size())) {
			entityComponentSignatures.resize(entityId + 1);
		}
	}
	else {
		// Reused an ID from the list of previously removed entities
		entityId = freeIDs.front();
		freeIDs.pop_front();
	}

	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	Logger::Log("Entity created with id = " + std::to_string(entityId));

	return entity;
}


void Registry::killEntity(Entity entity) {
	entitiesToBeDeleted.insert(entity);
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


void Registry::removeEntityFromSystems(Entity entity) {
	for (auto system : systems) {
		system.second->removeEntityFromSystem(entity);
	}
}


void Registry::updateRegistry() {
	// Processing the entites that are waiting to be created to the active Systems
	for (auto entity : entitiesToBeAdded) {
		addEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	// Process the entites that are waiting to be deleted from the active systems
	for (auto entity : entitiesToBeDeleted) {
		removeEntityFromSystems(entity);
		
		entityComponentSignatures[entity.getEntityId()].reset();

		// Make entity ID available to be reused
		freeIDs.push_back(entity.getEntityId());
	}
	entitiesToBeDeleted.clear();
}