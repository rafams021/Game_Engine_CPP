#ifndef ECS_H
#define ECS_H

#include <vector>
#include <bitset>

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
	// Returns the unique id of the component<T>
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};


class Entity
{
public:
	Entity(int id) : id(id) {};
	Entity(const Entity& entity) = default;
	int GetId() const;

	// Operators Overloading
	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }

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

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	// Defines the component type that entities must have to be considered by the system
	template <typename TComponent> void RequireComponent();

private:
	Signature componentSignature;
	std::vector<Entity> entities;

};


class Registry
{
public:

private:

};


template <typename TComponent>
void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}


#endif // !ECS_H

