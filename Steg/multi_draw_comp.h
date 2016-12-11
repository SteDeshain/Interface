//camera.h
//created by SteDeshain
//created on 2016/08/12
//latest updated on 2016/08/13
//MultiDrawComp: a DrawablwComp with not less than one destRect to be drawn
//the destRect from the father is inherited, which stands for CURRENT destRect for real in this class
//and so as sourceRect

#ifndef __MULTI_DRAW_COMP_H__
#define __MULTI_DRAW_COMP_H__

#include "SDL2/SDL.h"
#include "status.h"
#include "drawable_comp.h"

namespace steg
{

class MultiDrawComp: virtual public DrawableComp
{

public:
    //make a copy of destRects and sourceRects
    MultiDrawComp(const char* name, int x, int y, int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects,
                  int textureNum, const char* imgFiles, DrawableComp* attachedPlatform = NULL);
    MultiDrawComp(int x, int y, int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects, int textureNum, const char* imgFiles, DrawableComp* attachedPlatform = NULL);
    virtual ~MultiDrawComp();

	virtual void SetSourceRect(SDL_Rect sourceRect);
	virtual void SetSourceSize(int w = 0, int h = 0);

protected:
    int destNum;
    SDL_Rect* destRects = NULL;    //bunch of destRects, which is static, cannot be changed
                            //the destRects all are relative to drawRect's left-top
    SDL_Rect* sourceRects = NULL;

    //the original x, y is meaningless for this class
    //so I set x, y to the center of all destRects, and as well as the center of drawRect

    virtual DBG_Status Draw(Uint32 deltTick, Camera* camera);

	virtual DBG_Status InitInScene(Scene *scene);	//called after inserted into a Scene
	virtual DBG_Status DumpOutOfScene();			//called after thrown out by a Scene

    virtual void UpdateDestRect();  //don't need to update destRect, so I override it with a funvtion that does nothing

private:
    int drawWidth, drawHeight;

};

}

#endif // __MULTI_DRAW_COMP_H__
