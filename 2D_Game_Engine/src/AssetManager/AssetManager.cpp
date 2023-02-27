#include "AssetManager.h"
#include "../Logger/Logger.h"
#include <SDL_image.h>

AssetManager::AssetManager() {
	Logger::Log("AssetManager constructor called!");

}


AssetManager::~AssetManager() {
	clearAssets();
	Logger::Log("AssetManager destructor called!");

}


void AssetManager::clearAssets() {
	for (auto texture : textures) {
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();
}

void AssetManager::addTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filepath) {
	SDL_Surface* surface = IMG_Load(filepath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Add the texture to the map
	textures.emplace(assetId, texture);

	Logger::Log("New texture added to the Assets Manager with id = " + assetId);
}

SDL_Texture* AssetManager::getTexture(const std::string& assetId) {
	return textures[assetId];
}