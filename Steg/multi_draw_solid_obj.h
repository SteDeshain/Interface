#ifndef __MULTI_DRAW_SOLID_OBJ_H__
#define __MULTI_DRAW_SOLID_OBJ_H__

#include "multi_draw_comp.h"
#include "solid_obj.h"

namespace steg
{

class MultiDrawSolidObj: public MultiDrawComp, public SolidObj
{

public:
    MultiDrawSolidObj(SolidObjInfo info, int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects,
                      const char* imgFile, int textureNum = 1);

protected:
    virtual DBG_Status InitInScene(Scene* scene);
    virtual DBG_Status Draw(Uint32 deltTick, Camera* camera);

};

}

#endif  // __MULTI_DRAW_SOLID_OBJ_H__
