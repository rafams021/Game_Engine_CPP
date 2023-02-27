#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <map>
#include<string>
#include <SDL.h>

class AssetManager {
public:
	AssetManager();
	~AssetManager();

	void clearAssets();
	void addTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filepath);
	SDL_Texture* getTexture(const std::string& assetId);

private:
	std::map<std::string, SDL_Texture*> textures;
	// TODO: Create map for fonts
	// TODO: Create map for audio
};

#endif // !ASSETMANAGER_H
