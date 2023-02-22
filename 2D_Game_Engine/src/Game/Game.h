#ifndef GAME_H
#define GAME_H

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
	void SetUp();
	void Update();
	void Render();
	void Destroy();

	int windowWidth;
	int windowHeight;

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	int millisecsPreviusFrame = 0;
	bool isRunning;
};

#endif // !GAME_H
