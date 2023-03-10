#ifndef ECS_H
#define ECS_H

#include "../Logger/Logger.h"
#include <vector>
#include <bitset>
#include <set>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <deque>

const unsigned int MAX_COMPONENTS = 32;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Signature
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// We use a bitset (1s and 0s) to keep track of which components an entity has,
// and also helps keep track of which entities a system is interested in.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent
{
protected:
	static int nextId;
};

// Used to assign a unique id to a componetn type
template <typename T>
class Component: public IComponent
{
public:
	// Returns the unique id of the component<T>
	static int getComponentId() {
		static auto id = nextId++;
		return id;
	}
private:

};


class Entity
{
public:
	Entity(int id = 0) : id(id){};
	Entity(const Entity& entity) = default;
	int getEntityId() const;
	void deleteEntity();

	// Operators Overloading
	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }

	template <typename TComponent, typename ...TArgs> void addComponent(TArgs&& ...args);
	template <typename TComponent> void removeComponent();
	template <typename TComponent> bool hasComponent() const;
	template <typename TComponent> TComponent& getComponent() const;

	// Hold a pointer to the enity's owner registry
	class Registry* registry;

private:
	int id;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The system processes entities that contain a specific signature
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class System
{
public:
	System() = default;
	~System() = default;

	void addEntityToSystem(Entity entity);
	void removeEntityFromSystem(Entity entity);
	std::vector<Entity> getSystemEntities() const;
	const Signature& getComponentSignature() const;

	// Defines the component type that entities must have to be considered by the system
	template <typename TComponent> void requireComponent();

private:
	Signature componentSignature;
	std::vector<Entity> entities;

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The system processes entities that contain a specific signature
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IPool {
public:
	virtual ~IPool(){}
};

template <typename T>
class Pool: public IPool {
public:
	Pool(int size = 100) {
		data.resize(size);
	}

	virtual ~Pool() = default;

	bool isEmptyPool() const {
		return data.empty();
	}

	int getSizePool() const {
		return data.size();
	}

	void resizePool(int n) {
		data.resize(n);
	}

	void clearPool() {
		data.clear();
	}

	void addToPool(int index, T object) {
		data.push_back(object);
	}

	void setToPool(int index, T object) {
		data[index] = object;
	}

	T& getFromPool(int index) {
		return static_cast<T&>(data[index]);
	}

	T& operator [] (unsigned int index) {
		return data[index];
	}

private:
	std::vector<T> data;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registry
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The Registry manages the creation and destrution of entities, add sytems,
// and components
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Registry
{
public:
	Registry() {
		Logger::Log("Registry constructor called");
	}

	~Registry() {
		Logger::Log("Registry destructor called");
	}

	// The registry Update() finally processes the entities that are waiting to be added/deleted to the systems
	void updateRegistry();

	// Entity Managment
	Entity createEntity();
	void killEntity(Entity entity);

	// Component managment
	template <typename TComponent, typename ...TArgs> void addComponent(Entity entiy, TArgs&& ...args);
	template <typename TComponent> void removeComponent(Entity entity);
	template <typename TComponent> bool hasComponent(Entity entity) const;
	template <typename TComponent> TComponent& getComponent(Entity entity) const;


	// System managment
	template <typename TSystem, typename ...TArgs> void addSystem(TArgs&& ...args);
	template <typename TSystem> void removeSystem();
	template <typename TSystem> bool hasSystem() const;
	template <typename TSystem> TSystem& getSystem() const;

	// Add and Remove entities from their systems
	void addEntityToSystems(Entity entity);
	void removeEntityFromSystems(Entity entity);



private:
	int numEntities = 0;

	// Vector of component pools, each pool contains all the data for a certain component type
	// [Vector index = component type id]
	// [Pool index = entity id]
	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures per entity, saying which componet is turned "on" for a given entity
	// Vector[index = entity id]
	std::vector<Signature> entityComponentSignatures;

	// Map of active systems
	// [Map key = system type id]
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Set of entities that are flagged to be added or removed in the next registry Update()
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeDeleted;

	// List of free entity id's that were previously removed
	std::deque<int> freeIDs;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TComponent template implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TComponent, typename ...TArgs>
void Entity::addComponent(TArgs&& ...args) {
	registry->addComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}


template <typename TComponent>
void Entity::removeComponent() {
	registry->removeComponent<TComponent>(*this);
}


template<typename TComponent>
bool Entity::hasComponent() const {
	return registry->hasComponent<TComponent>(*this);
}


template <typename TComponent>
TComponent& Entity::getComponent() const {
	return registry->getComponent<TComponent>(*this);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TComponent template implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TComponent>
void System::requireComponent() {
	const auto componentId = Component<TComponent>::getComponentId();
	componentSignature.set(componentId);
}


template <typename TComponent, typename ...TArgs>
void Registry::addComponent(Entity entity, TArgs&& ...args) {
	const auto componentId = Component<TComponent>::getComponentId();
	const auto entityId = entity.getEntityId();

	if (componentId >= static_cast<int>(componentPools.size())) {
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	if (entityId >= componentPool->getSizePool()) {
		componentPool->resizePool(numEntities);
	}

	TComponent newCompnent(std::forward<TArgs>(args)...);

	componentPool->setToPool(entityId, newCompnent);

	entityComponentSignatures[entityId].set(componentId);

	Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}


template <typename TComponent>
void Registry::removeComponent(Entity entity) {
	const auto componentId = Component<TComponent>::getComponentId();
	const auto entityId = entity.getEntityId();

	entityComponentSignatures[entityId].set(componentId, false);

	Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}


template<typename TComponent>
bool Registry::hasComponent(Entity entity) const {
	const auto componentId = Component<TComponent>::getComponentId();
	const auto entityId = entity.getEntityId();

	return entityComponentSignatures[entityId].test(componentId);
}


template <typename TComponent> 
TComponent& Registry::getComponent(Entity entity) const {
	const auto componentId = Component<TComponent>::getComponentId();
	const auto entityId = entity.getEntityId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	return componentPool->getFromPool(entityId);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TSytem template implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TSystem, typename ...TArgs> 
void Registry::addSystem(TArgs&& ...args) {
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}


template <typename TSystem>
void Registry::removeSystem() {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}


template <typename TSystem> 
bool Registry::hasSystem() const {
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}


template <typename TSystem> 
TSystem& Registry::getSystem() const {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

#endif // !ECS_H

