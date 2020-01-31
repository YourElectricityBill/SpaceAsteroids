#include "Texture Manager.h"
#include "SDL_image.h"
#include "Debug Mode.h"


SDL_Texture* TextureManager::LoadTexture(const char* t_path, SDL_Renderer* t_tempRenderer)
{
	SDL_Surface* tempSurface = nullptr;
	SDL_Texture* resultTexture = nullptr;
    #if _DEBUG_MODE_ == 1
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading texture at path %s", t_path);
    #endif
	tempSurface = IMG_Load(t_path);
	resultTexture = SDL_CreateTextureFromSurface(t_tempRenderer, tempSurface);
	if (resultTexture == nullptr)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, "Attempt to load the texture at location %s resulted in a nullptr.", t_path);
		SDL_Delay(5000);
		SDL_Quit();
	}
    #if _DEBUG_MODE_ == 1
	else
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Texture has been loaded succesfully!");
	}
    #endif
	SDL_FreeSurface(tempSurface); // Removes the memory that the surface pointer points to.
	tempSurface = nullptr;
	if (tempSurface != nullptr) // It's extremely unlikely scenario though, but it won't hurt to keep record of such cases if they will ever occur
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Error while attempting to free surface in function LoadTexture(), possible memory leak");
	}

	return resultTexture;
}