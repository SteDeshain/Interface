//sources_load.h
//created by SteDeshain
//created on 2016/08/02
//latest updated on 2016/08/03
//function tools to load various of sources in SteDeshain engine driven by SDL2.0

#ifndef __SOURCES_LOAD__
#define __SOURCES_LOAD__

#include <cstring>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "status.h"
#include "log.h"

namespace steg
{

SDL_Texture* GetTexture(SDL_Renderer* render, const char* file);	//with access SDL_TEXTUREACCESS_STATIC

SDL_Texture* GetStreamingTexture(SDL_Renderer* render, const char* file, SDL_Window* window);

SDL_Texture* GetTargetTexture(SDL_Renderer* render, int w, int h, SDL_Window* window);
SDL_Texture* GetTargetTexture(SDL_Renderer* render, SDL_Window* window);	//get a texture having the same size with the MainWindow

SDL_Texture* GetColorTexture(SDL_Renderer* render, SDL_Point textureSize, SDL_Color color, float transparency);
SDL_Texture* GetButtonTexture(SDL_Renderer* render, SDL_Point buttonSize, SDL_Color color);

}

#endif
