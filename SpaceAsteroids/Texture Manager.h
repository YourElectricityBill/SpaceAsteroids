#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include "SDL.h"

class TextureManager
{
public:
	static SDL_Texture* LoadTexture(const char* t_path, SDL_Renderer* t_tempRenderer); //There's no point to have a separate texture manager for each instance of an object

};
 
#endif