#include "multi_draw_comp.h"

namespace steg
{

MultiDrawComp::MultiDrawComp(int x, int y, int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects, int textureNum, const char* imgFiles, DrawableComp* attachedPlatform)
    :DrawableComp(textureNum, imgFiles, attachedPlatform), destNum(destNum), destRects(destRects), sourceRects(sourceRects)
{
    SetRelativePos(x, y);

    if(NULL == destRects || NULL ==sourceRects)
    {
        ENG_LogError("bad argument: destRects or sourceRects can't be NULL!");
        drawWidth = drawHeight = 0;
    }
    else
    {
        int minX = destRects[0].x;
        int maxX = destRects[0].x + destRects[0].w;
        int minY = destRects[0].y;
        int maxY = destRects[0].y + destRects[0].h;
        for(int i = 0; i < destNum; i++)
        {
            if(destRects[i].x < minX)
                minX = destRects[i].x;

            if(destRects[i].x + destRects[i].w > maxX)
                maxX = destRects[i].x + destRects[i].w;

            if(destRects[i].y < minY)
                minY = destRects[i].y;

            if(destRects[i].y + destRects[i].h > maxY)
                maxY = destRects[i].y + destRects[i].h;
        }
        drawWidth = maxX - minX;
        drawHeight = maxY - minY;
    }
}

MultiDrawComp::~MultiDrawComp()
{

}

void MultiDrawComp::SetSourceRect(SDL_Rect sourceRect)
{
    //you can't change the sourceRect in MultiDrawComp
}

void MultiDrawComp::SetSourceSize(int w, int h)
{
    //you can't change the sourceRect in MultiDrawComp
}

DBG_Status MultiDrawComp::Draw(Uint32 deltTick, Camera* camera)
{
    DBG_Status status = DBG_OK;

    for(int i = 0; i < destNum; i++)
    {
        destRect = SDL_Rect{destRects[i].x + x - drawWidth / 2,
                            destRects[i].y + y - drawHeight / 2,
                            destRects[i].w,
                            destRects[i].h};
        sourceRect = sourceRects[i];
        status |= DrawableComp::Draw(deltTick, camera);
    }

    return status;
}

DBG_Status MultiDrawComp::InitInScene(Scene *scene)
{
	DBG_Status status = DBG_OK;

	status |= DrawableComp::InitInScene(scene);
	SetDrawSize(drawWidth, drawHeight);

	return status;
}

void MultiDrawComp::UpdateDestRect()
{
    //do nothing, avoid modifying destRect
}

}
