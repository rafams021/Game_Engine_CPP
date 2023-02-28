#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

Game::Game()
{
	isRunning = false;
	registry = std::make_unique<Registry>();
	assetManager = std::make_unique<AssetManager>();
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


void::Game::LoadLevel(int level) {
	// Add the system that need to be processed in our game
	registry->addSystem<MovementSystem>();
	registry->addSystem<RenderSystem>();
	registry->addSystem<AnimationSystem>();

	// Adding assets to the AssetsManager
	assetManager->addTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetManager->addTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetManager->addTexture(renderer, "chopper-image", "./assets/images/chopper.png");
	assetManager->addTexture(renderer, "radar-image", "./assets/images/radar.png");
	assetManager->addTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

	// Load the Tilemap
	double tileScale = 2.0;
	int tileSize = 32;
	int mapNumRows = 20;
	int mapNumCols = 25;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = registry->createEntity();
			tile.addComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			tile.addComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}
	mapFile.close();

	// Create an  entities
	Entity chopper = registry->createEntity();
	chopper.addComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.addComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	chopper.addComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.addComponent<AnimationComponent>(2, 15, true);

	Entity radar = registry->createEntity();
	radar.addComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.addComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.addComponent<SpriteComponent>("radar-image", 64, 64, 1);
	radar.addComponent<AnimationComponent>(8, 5, true);

	Entity tank = registry->createEntity();
	tank.addComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.addComponent<RigidBodyComponent>(glm::vec2(25.0, 0.0));
	tank.addComponent<SpriteComponent>("tank-image", 32, 32, 3);

	Entity truck = registry->createEntity();
	truck.addComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	truck.addComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
	truck.addComponent<SpriteComponent>("truck-image", 32, 32, 4);
}

void Game::SetUp() {
	LoadLevel(1);
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

	// Update the registry to porcess the entities that are waiting to be created/deleted
	registry->updateRegistry();

	// Invoke all the system that need to be update
	registry->getSystem<MovementSystem>().updateMovement(deltaTime);

	registry->getSystem<AnimationSystem>().updateAnimation();
	// TODO: registry->getSystem<CollisionSystem>()->updateCollision();

}

void Game::Render() {
	// Render Window with color black
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// Invoke all the system that need to render
	registry->getSystem<RenderSystem>().updateRender(renderer, assetManager);

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