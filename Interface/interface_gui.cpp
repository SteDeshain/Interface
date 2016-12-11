#include "interface_gui.h"
#include "tools.h"
#include "stel.h"
#include "load_comps.h"

namespace interface
{

//GUI::GUI(const char* name, int x, int y, int drawIndex,
//         int textureNum, const char* imgFile, steg::Canvas* canvas)
//    :steg::GUI(x, y, textureNum, imgFile, canvas), Name(name)
//{
//    this->drawIndex = drawIndex;
//}
//
//GUI::GUI(const char* name, int x, int y, int drawIndex,
//         SDL_Point picSize, SDL_Color color, float transparency, steg::Canvas* canvas)
//    :steg::GUI(x, y, picSize, color, transparency, canvas), Name(name)
//{
//    this->drawIndex = drawIndex;
//}

//Label::Label(const char* name, int x, int y, steg::LabelAnchor anchor,
//             const char* text, TTF_Font* font, SDL_Color color, int drawIndex, steg::Canvas* canvas)
//    :steg::Label(x, y, anchor, text, font, color, drawIndex, canvas), Name(name)
//{
//}

Canvas::Canvas(const char* name, const char* imgFile, float transparency,
               SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible)
    :steg::Canvas(name, imgFile, transparency, viewRect, canvasSize, motherCanvas, startVisible)//, steg::GUI(1, imgFile, motherCanvas)
{
    if(this->name.empty())
    {
        //no script
    }
    else
    {
        std::string filePath = "scripts/comps/";
        DBG_Status status = NewLuaProxy(this->name, filePath, this);
    }
}

Canvas::Canvas(const char* name, SDL_Color color, float transparency,
               SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible)
    :steg::Canvas(name, color, transparency, viewRect, canvasSize, motherCanvas, startVisible)//, steg::GUI(1, NULL, motherCanvas)
{
    if(this->name.empty())
    {
        //no script
    }
    else
    {
        std::string filePath = "scripts/comps/";
        DBG_Status status = NewLuaProxy(this->name, filePath, this);
    }
}

//DBG_Status Canvas::InitInScene(steg::Scene* scene)
//{
//    DBG_Status status = DBG_OK;
//
//    status |= steg::Canvas::InitInScene(scene);
//
////    visible = startVisible;
////    originVisible = startVisible;
//    SetVisible(startVisible);
//
//    return status;
//}

}
