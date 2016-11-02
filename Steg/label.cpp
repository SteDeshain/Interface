#include "label.h"
#include "color.h"
#include "log.h"
#include "status.h"

namespace steg
{

Label::Label(int x, int y, LabelAnchor anchor, const char* text, TTF_Font* font, SDL_Color color, int drawIndex, Canvas* canvas)
    :GUI(0, 0, drawIndex, canvas), anchor(anchor), text(text), font(font), color(color)
{
    anchorPoint = SDL_Point{x, y};
}

Label::~Label()
{

}

DBG_Status Label::LoadTexture()
{
    DBG_Status status = DBG_OK;

#ifdef LINUX
    #ifdef DEBUG
//    SDL_Surface* fontSurface = TTF_RenderText_Solid(font, text, color);
//    SDL_Surface* fontSurface = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Surface* fontSurface = TTF_RenderUTF8_Blended(font, text, color);
    #else
//    SDL_Surface* fontSurface = TTF_RenderText_Blended(font, text, color);
    SDL_Surface* fontSurface = TTF_RenderUTF8_Blended(font, text, color);
    #endif // DEBUG
#endif  // LINUX
#ifdef WINDOWS
    SDL_Surface* fontSurface = TTF_RenderUTF8_Blended(font, text, color);
#endif // WINDOWS
    if(NULL == fontSurface)
    {
        ENG_LogErrors("Cannot create font surface for a Label. ", TTF_GetError());
        SDL_ClearError();
        status |= DBG_NULL_PTR | DBG_TTF_ERR;
    }
    else
    {
        textures[0] = NULL;
        textures[0] = SDL_CreateTextureFromSurface(motherScene->render, fontSurface);
        if(NULL == textures[0])
        {
            ENG_LogErrors("Cannot create texture for Label. ", SDL_GetError());
            SDL_ClearError();
            status |= DBG_NULL_PTR | DBG_SDL_ERR;
        }
        else
        {
            currentTexture = textures[0];
        }
        SDL_FreeSurface(fontSurface);
    }

    //init rects
    sourceRect.x = sourceRect.y = 0;
    int w, h;
    if(SDL_QueryTexture(currentTexture, NULL, NULL, &w, &h))
    {
        ENG_LogError(SDL_GetError());
        SDL_ClearError();
        SetSourceSize();
        SetDrawSize();
        status |= DBG_SDL_ERR;
    }
    else
    {
        SetSourceSize(w, h);
		SetDrawSize(w, h);
    }
    SetRelativePos(x, y);

    switch(anchor)
    {
    case LeftTop:
        SetRelativeLeftTop(anchorPoint);
        break;
    case RightTop:
        SetRelativeLeftTop(anchorPoint.x - drawRect.w, anchorPoint.y);
        break;
    case Center:
    default:
        SetRelativePos(anchorPoint);
        break;
    }

    entireSize = SDL_Point{sourceRect.w, sourceRect.h};
    drawSize = entireSize;

    return status;
}

DBG_Status Label::ReloadTexture(const char* newText)
{
    DBG_Status status = DBG_OK;

    if(NULL != newText)
    {
        text = newText;
    }

    if(textures)
    {
        if(textures[0])
        {
            currentTexture = NULL;
            SDL_DestroyTexture(textures[0]);
            status |= LoadTexture();
        }
        else
            status |= DBG_NULL_PTR;
    }
    else
        status |= DBG_NULL_PTR;

    return status;
}

DBG_Status Label::InitInScene(Scene *scene)
{
    DBG_Status status = DBG_OK;

    status |= GUI::InitInScene(scene);
    if(status & DBG_REP_OPR)
        return status;

    visible = true;

    status |= LoadTexture();

    SDL_GetTextureAlphaMod(currentTexture, &textureAlpha);

    return status;
}

DBG_Status Label::DumpOutOfScene()
{
    DBG_Status status = DBG_OK;

    status |= GUI::DumpOutOfScene();
    return status;
}

}
