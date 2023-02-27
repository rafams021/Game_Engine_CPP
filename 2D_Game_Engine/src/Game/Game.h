#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include <SDL.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game
{
public:
	Game();
	~Game();
	void Initialize();
	void Run();
	void ProcessInput();
	void LoadLevel(int level);
	void SetUp();
	void Update();
	void Render();
	void Destroy();

	int windowWidth;
	int windowHeight;

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetManager> assetManager;
	int millisecsPreviusFrame = 0;
	bool isRunning;
};

#endif // !GAME_H
