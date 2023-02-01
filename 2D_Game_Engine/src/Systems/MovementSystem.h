#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTME_H

class MovementSystem : public System
{
public:
	MovementSystem : public System();
	void Update();
	~MovementSystem : public System();

private:

};

MovementSystem : public System::MovementSystem : public System(){
	// TODO:
	// RequiereComponent<TransformComponent();
	// RequiereComponent<...>();
}

void Update() {
	// TODO:
	// Loop al entities that the system is interested in
	for (auto& entity : GetEntities()) {
		// Update entity position based on its velocity

	}
}

MovementSystem : public System::~MovementSystem : public System(){

}

#endif // !MOVEMENTSYSTEM_H
