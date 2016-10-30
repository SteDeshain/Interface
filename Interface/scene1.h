//camera.h
//created by SteDeshain
//created on 2016/08/17
//latest updated on 2016/08/17
//test scene

#ifndef __SCENE1_H__
#define __SCENE1_H__

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "scene.h"
#include "status.h"
#include "multi_draw_comp.h"
#include "drawable_comp.h"
#include "label.h"
#include "solid_obj.h"
#include "multi_draw_solid_obj.h"
#include "character.h"

namespace gameTest
{

using namespace steg;

class Scene1: public steg::Scene
{

public:
    TTF_Font* debugFont;
    TTF_Font* menuFont;

    MultiDrawComp* ground;
    DrawableComp* mountain;
    DrawableComp* tiles;
    Label* label;
    Label* camPos;
//    SolidObj* character;
    Character* character;
    SolidObj* char2;
    SolidObj* char3;
    SolidObj* char4;
    char buffer[128];
    SDL_Rect* rects;
    SDL_Rect* sRects;
    SDL_Rect* rects2;
    SDL_Rect* sRects2;
    MultiDrawSolidObj* platform;
    Canvas* motherCanv;
    Canvas* canv;
    Canvas* childCanv;
    DragButton* but2;

    //for debug
    Label* compCount;
    Label* drawableCount;
    Label* GUICount;
    Label* SolidCount;
    Label* mapInfo;
    Label* posValue;
    Label* charsFrontObjsNum;
    Label* resortCount;
    int resortCt;

public:
    Scene1(int mapWidthNum, int mapHeightNum, SDL_Renderer* render);
    virtual ~Scene1();

    virtual DBG_Status InitScene();
    virtual DBG_Status HandleInput();
    //debug use
    virtual DBG_Status Update(Uint32 deltTick);

    //temp
    virtual DBG_Status ResortSolidObjs();

protected:

private:

};

}

#endif // __SCENE1_H__
