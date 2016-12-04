#include "interface_comp.h"

namespace interface
{

Name::Name(const char* name)
    :name(name)
{
}

const char* Name::GetName()
{
    return name.c_str();
}

GameComp::GameComp(const char* name)
    :steg::GameComp(), Name(name)
{
}

DrawableComp::DrawableComp(const char* name, int x, int y, unsigned int textureNum, const char* imgFiles,
                           DrawableComp* attachedPlatform)
    :steg::DrawableComp(x, y, textureNum, imgFiles, attachedPlatform), Name(name)
{
}

MultiDrawComp::MultiDrawComp(const char* name,
                             int x, int y, int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects,
                             int textureNum, const char* imgFiles, DrawableComp* attachedPlatform)
    :steg::MultiDrawComp(x, y, destNum, destRects, sourceRects, textureNum, imgFiles, attachedPlatform), Name(name)
{
}

SolidObj::SolidObj(const char* name, steg::SolidObjInfo info, const char* imgFile, int textureNum)
    :steg::SolidObj(info, imgFile, textureNum), Name(name)
{
}

MultiDrawSolidObj::MultiDrawSolidObj(const char* name, steg::SolidObjInfo info,
                                     int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects,
                                     const char* imgFile, int textureNum)
    :steg::MultiDrawSolidObj(info, destNum, destRects, sourceRects, imgFile, textureNum), Name(name)
{
}

}
