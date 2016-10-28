#include "sources_load.h"
#include "game.h"

namespace steg
{

//>>**************************** internal use function ****************************<<//

static SDL_Texture *_GetTexture(SDL_Renderer *render, const char *file, SDL_Window *window, SDL_TextureAccess access)
{
	SDL_Texture *res = NULL;

	if(NULL == render || NULL == file || (NULL == window && SDL_TEXTUREACCESS_STREAMING == access))
	{
		ENG_LogError("Argument can't be the NULL pointer!");
		return NULL;
	}

	SDL_Surface *loadedSurface = IMG_Load(file);
	if(NULL == loadedSurface)
	{
		ENG_LogError(IMG_GetError());
		SDL_ClearError();
		return NULL;
	}


	SDL_Surface *windowSurface;
	SDL_Surface *formattedSurface;
	switch(access)
	{
	case SDL_TEXTUREACCESS_STREAMING:
		windowSurface = SDL_GetWindowSurface(window);
		if(NULL == windowSurface)
		{
			ENG_LogError(SDL_GetError());
			SDL_ClearError();
			SDL_FreeSurface(loadedSurface);
			return NULL;
		}

		formattedSurface = SDL_ConvertSurface(loadedSurface, windowSurface->format, 0);
		if(NULL == formattedSurface)
		{
			ENG_LogError(SDL_GetError());
			SDL_ClearError();
			SDL_FreeSurface(loadedSurface);
			SDL_FreeSurface(windowSurface);
			return NULL;
		}

		res = SDL_CreateTexture(render,
								SDL_GetWindowPixelFormat(window),
								SDL_TEXTUREACCESS_STREAMING,
								formattedSurface->w,
								formattedSurface->h);
		if(NULL == res)
		{
			ENG_LogError(SDL_GetError());
			SDL_ClearError();
			SDL_FreeSurface(formattedSurface);
			SDL_FreeSurface(windowSurface);
			SDL_FreeSurface(loadedSurface);
			return NULL;
		}

		void* nPixels;
    	int nPitch;
		SDL_LockTexture(res, NULL, &nPixels, &nPitch);
		memcpy(nPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);
		SDL_UnlockTexture(res);

		SDL_FreeSurface(formattedSurface);
		SDL_FreeSurface(windowSurface);
		SDL_FreeSurface(loadedSurface);

		break;

	case SDL_TEXTUREACCESS_TARGET:
		ENG_LogError("If you want to get a render target texture, please use the GetTargetTexture() function!");
		SDL_FreeSurface(loadedSurface);
		break;

	case SDL_TEXTUREACCESS_STATIC:
	default:
		res = SDL_CreateTextureFromSurface(render, loadedSurface);
		if(NULL == res)
		{
			ENG_LogError(SDL_GetError());
			SDL_ClearError();
			SDL_FreeSurface(loadedSurface);
			return NULL;
		}
		SDL_FreeSurface(loadedSurface);
		break;
	}

    return res;
}

//>>**************************** Get Non-Target Texture ****************************<<//

SDL_Texture *GetTexture(SDL_Renderer *render, const char *file)
{
	return _GetTexture(render, file, NULL, SDL_TEXTUREACCESS_STATIC);
}

SDL_Texture *GetStreamingTexture(SDL_Renderer *render, const char *file, SDL_Window *window)
{
	return _GetTexture(render, file, window, SDL_TEXTUREACCESS_STREAMING);
}

//>>**************************** Get Target Texture ****************************<<//

SDL_Texture *GetTargetTexture(SDL_Renderer *render, int w, int h, SDL_Window *window)
{
	SDL_Texture *res = NULL;

	if(NULL == render || w <= 0 || h <= 0)
	{
		ENG_LogError("Argument failure!");
		return NULL;
	}

	res = SDL_CreateTexture(render,
							SDL_GetWindowPixelFormat(window),
							SDL_TEXTUREACCESS_TARGET,
							w, h);
	if(NULL == res)
	{
		ENG_LogError(SDL_GetError());
		SDL_ClearError();
		return NULL;
	}

	return res;
}

SDL_Texture *GetTargetTexture(SDL_Renderer *render, SDL_Window *window)
{
	SDL_Texture *res = NULL;

	if(NULL == render)
	{
		ENG_LogError("Argument can't be the NULL pointer!");
		return NULL;
	}

	SDL_Surface *windowSurface = SDL_GetWindowSurface(window);
	if(NULL == windowSurface)
	{
		ENG_LogError(SDL_GetError());
		SDL_ClearError();
		return NULL;
	}

	res = SDL_CreateTexture(render,
							SDL_GetWindowPixelFormat(window),
							SDL_TEXTUREACCESS_TARGET,
							windowSurface->w,
							windowSurface->h);

	SDL_FreeSurface(windowSurface);		//Do not free the surface returned by SDL_GetWindowSurface()? noted from SDL2.0 wiki
										//It's bullshit, just free it unless you want it to present on screen.

	if(NULL == res)
	{
		ENG_LogError(SDL_GetError());
		SDL_ClearError();
		return NULL;
	}

	return res;

}

//>>**************************** Get Color Texture ****************************<<//

SDL_Texture* GetColorTexture(SDL_Renderer* render, SDL_Point textureSize, SDL_Color color, float transparency)
{
	SDL_Texture* tmp = NULL;
	SDL_Texture* res = NULL;

	if(transparency < 0)
        transparency = 0;
    else if(transparency > 1.0f)
        transparency = 1.0f;

    tmp = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, textureSize.x, textureSize.y);
//    res = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, textureSize.x, textureSize.y);
    if(tmp == NULL)// || res == NULL)
    {
        ENG_LogErrors("Cannot create SDL_Texture from SDL_CreateTexture: ", SDL_GetError());
        SDL_ClearError();
    }
    else
    {
        //set tmp texture
        SDL_SetTextureBlendMode(tmp, SDL_BLENDMODE_BLEND);

        SDL_Texture* originTarget = SDL_GetRenderTarget(render);
        SDL_SetRenderTarget(render, tmp);

        SDL_Color originDrawColor;
        SDL_GetRenderDrawColor(render, &originDrawColor.r, &originDrawColor.g, &originDrawColor.b, &originDrawColor.a);

        SDL_SetRenderDrawColor(render, color.r, color.g, color.b, (Uint8)(transparency * 255));
        SDL_RenderClear(render);

        SDL_SetRenderDrawColor(render, originDrawColor.r, originDrawColor.g, originDrawColor.b, originDrawColor.a);

        SDL_SetRenderTarget(render, originTarget);

        //set res texture
//        SDL_SetTextureBlendMode(res, SDL_BLENDMODE_BLEND);
//        Uint32 srcColor = SDL_MapRGBA(SDL_GetWindowSurface(Game::mainWindow)->format,
//                                      color.r, color.g, color.b, (Uint8)(transparency * 255));
//        SDL_Surface* surf = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
//        Uint32 srcColor = SDL_MapRGBA(SDL_GetWindowSurface(Game::mainWindow)->format, 255, 255, 128, 255);
//        Uint32 srcColor = SDL_MapRGBA(surf->format, 255, 255, 128, 0);
//        Uint32 srcColor = SDL_MapRGBA(surf->format, 128, 0, 0, 255);
                                                   //a   b    g    r
//        Uint32 srcColors[textureSize.x * textureSize.y];

//        SDL_UpdateTexture(res, NULL, srcColors, textureSize.x);

//        Uint32* srcColors = new Uint32[textureSize.x * textureSize.y];
//        memset(srcColors, 0, sizeof(Uint32) * textureSize.x * textureSize.y);
//
//        SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(srcColors, textureSize.x, textureSize.y, 32,
//                                                     textureSize.x, 0, 0, 0, 0x000000FF);
//        if(surf == NULL)
//        {
//        }
//        else
//        {
//        }
//        Uint32 srcColor = SDL_MapRGBA(surf->format, color.r, color.g, color.b, (Uint8)(transparency * 255));
//        for(int i = 0; i < textureSize.x * textureSize.y; i++)
//        {
//            srcColors[i] = srcColor;
//        }
//
//        res = SDL_CreateTextureFromSurface(render, surf);
//        if(res == NULL)
//        {
//            ENG_LogErrors("Cannot create SDL_Texture from SDL_CreateTextureFromSurface: ", SDL_GetError());
//            SDL_ClearError();
//        }
//
//        SDL_UpdateTexture(res, NULL, srcColors, textureSize.x);
    }

	return tmp;
}

SDL_Texture* GetButtonTexture(SDL_Renderer* render, SDL_Point buttonSize, SDL_Color color)
{
    SDL_Texture* res = NULL;

    SDL_Texture* originTarget = SDL_GetRenderTarget(render);
    SDL_Color originDrawColor;
    SDL_GetRenderDrawColor(render,
                           &(originDrawColor.r),
                           &(originDrawColor.g),
                           &(originDrawColor.b),
                           &(originDrawColor.a));

    res = SDL_CreateTexture(render,
                            SDL_PIXELFORMAT_RGBA8888,
                            SDL_TEXTUREACCESS_TARGET,
                            buttonSize.x * 2,
                            buttonSize.y);

    SDL_SetTextureBlendMode(res, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(render, res);

    //draw backgeound
    SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
    SDL_RenderClear(render);

    //draw right button
    SDL_Color tmpColor;
    tmpColor = SDL_Color{color.r > 255 - 70 ? 255 : color.r + 70,
                         color.g > 255 - 70 ? 255 : color.g + 70,
                         color.b > 255 - 70 ? 255 : color.b + 70,
                         color.a};
    SDL_SetRenderDrawColor(render, tmpColor.r, tmpColor.g, tmpColor.b, tmpColor.a);
    SDL_RenderDrawLine(render, 0, 0, buttonSize.x - 1, 0);
    SDL_RenderDrawLine(render, 0, 1, buttonSize.x - 2, 1);
    SDL_RenderDrawLine(render, 0, 0, 0, buttonSize.y - 1);
    SDL_RenderDrawLine(render, 1, 0, 1, buttonSize.y - 2);

    SDL_RenderDrawLine(render, buttonSize.x + 1, buttonSize.y - 1, buttonSize.x * 2, buttonSize.y - 1);
    SDL_RenderDrawLine(render, buttonSize.x + 2, buttonSize.y - 2, buttonSize.x * 2, buttonSize.y - 2);
    SDL_RenderDrawLine(render, buttonSize.x * 2 - 1, 0, buttonSize.x * 2 - 1, buttonSize.y);
    SDL_RenderDrawLine(render, buttonSize.x * 2 - 2, 0, buttonSize.x * 2 - 2, buttonSize.y);

    //draw left button
    tmpColor = SDL_Color{color.r < 70 ? 0 : color.r - 70,
                         color.g < 70 ? 0 : color.g - 70,
                         color.b < 70 ? 0 : color.b - 70,
                         color.a};
    SDL_SetRenderDrawColor(render, tmpColor.r, tmpColor.g, tmpColor.b, tmpColor.a);
    SDL_RenderDrawLine(render, buttonSize.x, 0, buttonSize.x * 2 - 1, 0);
    SDL_RenderDrawLine(render, buttonSize.x, 1, buttonSize.x * 2 - 2, 1);
    SDL_RenderDrawLine(render, buttonSize.x, 0, buttonSize.x, buttonSize.y - 1);
    SDL_RenderDrawLine(render, buttonSize.x + 1, 1, buttonSize.x + 1, buttonSize.y - 2);

    SDL_RenderDrawLine(render, 1, buttonSize.y - 1, buttonSize.x, buttonSize.y - 1);
    SDL_RenderDrawLine(render, 2, buttonSize.y - 2, buttonSize.x, buttonSize.y - 2);
    SDL_RenderDrawLine(render, buttonSize.x - 1, 1, buttonSize.x - 1, buttonSize.y);
    SDL_RenderDrawLine(render, buttonSize.x - 2, 2, buttonSize.x - 2, buttonSize.y);

    SDL_SetRenderDrawColor(render,
                           originDrawColor.r,
                           originDrawColor.g,
                           originDrawColor.b,
                           originDrawColor.a);
    SDL_SetRenderTarget(render, originTarget);

    return res;
}

}
