#include "interface_gui.h"

namespace interface
{

GUI::GUI(const char* name, int x, int y, int drawIndex,
         int textureNum, const char* imgFile, steg::Canvas* canvas)
    :steg::GUI(x, y, textureNum, imgFile, canvas), Name(name)
{
    this->drawIndex = drawIndex;
}

GUI::GUI(const char* name, int x, int y, int drawIndex,
         SDL_Point picSize, SDL_Color color, float transparency, steg::Canvas* canvas)
    :steg::GUI(x, y, picSize, color, transparency, canvas), Name(name)
{
    this->drawIndex = drawIndex;
}

Label::Label(const char* name, int x, int y, steg::LabelAnchor anchor,
             const char* text, TTF_Font* font, SDL_Color color, int drawIndex, steg::Canvas* canvas)
    :steg::Label(x, y, anchor, text, font, color, drawIndex, canvas), Name(name)
{
}

Canvas::Canvas(const char* name, const char* imgFile, float transparency,
               SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas)
    :steg::Canvas(imgFile, transparency, viewRect, canvasSize, motherCanvas), Name(name)
{
}

Canvas::Canvas(const char* name, SDL_Color color, float transparency,
               SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas)
    :steg::Canvas(color, transparency, viewRect, canvasSize, motherCanvas), Name(name)
{
}

}
