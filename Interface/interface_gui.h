#ifndef __INTERFACE_GUI_H__
#define __INTERFACE_GUI_H__

#include "gui.h"
#include "label.h"
#include "interface_comp.h"

namespace interface
{

//class GUI: public steg::GUI, public Name
//{
//
//public:
//    //picture mode
//    GUI(const char* name, int x, int y, int drawIndex,
//        int textureNum, const char* imgFile, steg::Canvas* canvas);
//    //color mode
//    GUI(const char* name, int x, int y, int drawIndex,
//        SDL_Point picSize, SDL_Color color, float transparency, steg::Canvas* canvas);
//
//};

//class Label: public steg::Label
//{
//
//public:
//    Label(const char* name, int x, int y, steg::LabelAnchor anchor,
//          const char* text, TTF_Font* font, SDL_Color color, int drawIndex, steg::Canvas* canvas);
//
//};

class Canvas: public steg::Canvas
{

public:
    //picture mode
    Canvas(const char* name, const char* imgFile, float transparency,
           SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible);
    //color mode
    Canvas(const char* name, SDL_Color color, float transparency,
           SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible);

protected:
//    DBG_Status InitInScene(steg::Scene* scene);

private:
//    bool startVisible;

};

}

#endif // __INTERFACE_GUI_H__
