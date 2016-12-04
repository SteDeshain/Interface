#ifndef __INTERFACE_COMP_H__
#define __INTERFACE_COMP_H__

#include "game_comp.h"
#include "drawable_comp.h"
#include "multi_draw_comp.h"
#include "solid_obj.h"
#include "multi_draw_solid_obj.h"
#include <string>

namespace interface
{

class Name
{

public:
    Name(const char* name);

    const char* GetName();

protected:
    std::string name;

};

class GameComp: public steg::GameComp, public Name
{

public:
    GameComp(const char* name);

};

class DrawableComp: public steg::DrawableComp, public Name
{

public:
    DrawableComp(const char* name, int x, int y, unsigned int textureNum, const char* imgFiles,
                 DrawableComp* attachedPlatform);

};

class MultiDrawComp: public steg::MultiDrawComp, public Name
{

public:
    MultiDrawComp(const char* name,
                  int x, int y, int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects,
                  int textureNum, const char* imgFiles, DrawableComp* attachedPlatform);

};

class SolidObj: public steg::SolidObj, public Name
{

public:
    SolidObj(const char* name, steg::SolidObjInfo info, const char* imgFile, int textureNum);

};

class MultiDrawSolidObj: public steg::MultiDrawSolidObj, public Name
{

public:
    MultiDrawSolidObj(const char* name, steg::SolidObjInfo info, int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects,
                      const char* imgFile, int textureNum);

};

}

#endif // __INTERFACE_COMP_H__
