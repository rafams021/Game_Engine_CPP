#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>

Game::Game()
{
	isRunning = false;
	registry = std::make_unique<Registry>();
	Logger::Log("Game Constructor Called!");
}

Game::~Game()
{
	Logger::Log("Game Destructor Called!");
}

void Game::Initialize() {
	if (!SDL_Init(SDL_INIT_EVERYTHING)) { // Return !0 if everithing is ok
		
		// Resolution of screen game

		SDL_DisplayMode displayMode;
		SDL_GetCurrentDisplayMode(0, &displayMode);
		windowWidth = displayMode.w;
		windowHeight = displayMode.h;
		
		/*********************************
		*
		* Create Window
		*
		*********************************/

		window = SDL_CreateWindow(NULL, 
								  SDL_WINDOWPOS_CENTERED, 
								  SDL_WINDOWPOS_CENTERED, 
								  windowWidth,
								  windowHeight,
								  SDL_WINDOW_BORDERLESS);
		if (!window) {
			Logger::Err("Error creating SDL window.");
			return;
		}
		
		/*********************************
		* 
		* Create Render
		* 
		*********************************/

		// Last parameter try to use Hardware Acceleration when possible and VSync to prevent screen tearing
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!renderer) {
			Logger::Err("Error creating SDL renderer.");
			return;
		}

		// Set Window to FullScreen

		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

		// Everithing is good to Run
		isRunning = true;

	}
	else {
		Logger::Err("Error initializing SDL.");
		return;
	}

}

void Game::ProcessInput() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type)
		{
		// User close window
		case SDL_QUIT:
			isRunning = false;
			break;
		// User press ESC button
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
			break;
		}
	}
}


void Game::SetUp() {
	// Create some entities
	Entity tank = registry->createEntity();
	Entity truck = registry->createEntity();
}


void Game::Update() {
	//  If we are too fast, wait until we reach MILLISECS_PER_FRAME
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviusFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	// The difference in ticks since the last frame converted in seconds
	double deltaTime = (SDL_GetTicks() - millisecsPreviusFrame) / 1000.0f;

	// Store current frame time.
	millisecsPreviusFrame = SDL_GetTicks();

	// TODO
}

void Game::Render() {
	// Render Window with color black
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// TODO

	SDL_RenderPresent(renderer);
}

void Game::Run() {
	SetUp();
	while (isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}

void Game::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}