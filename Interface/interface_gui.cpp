#include "interface_gui.h"
#include "tools.h"
#include "stel.h"
#include "load_comps.h"

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
               SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible)
    :steg::Canvas(imgFile, transparency, viewRect, canvasSize, motherCanvas), Name(name), startVisible(startVisible)
{
    if(this->name.empty())
    {
        //no script
    }
    else
    {
    }
}

Canvas::Canvas(const char* name, SDL_Color color, float transparency,
               SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible)
    :steg::Canvas(color, transparency, viewRect, canvasSize, motherCanvas), Name(name), startVisible(startVisible)
{
    if(this->name.empty())
    {
        //no script
    }
    else
    {
        LegalizeName(this->name); //the name is legal now
        std::string className = GetClassName(this->name);

        std::string filePath = "scripts/comps/";
        filePath += className;
        filePath += ".lua";
//        DBG_Status status = NewLuaProxy(this->name, filePath, this);

        //first, check naming collision
        steg::PLuaPushNil();                                // +1
        steg::PLuaPushFromTable_J(this->name.c_str());      // +1, actually, push from global
        while(!steg::PLuaTopIsNil())
        {
            NameAddOne(this->name);
            steg::PLuaPop();                                // -1
            steg::PLuaPushFromTable_J(this->name.c_str());  // +1
        }
        steg::PLuaPop(2);                                   // -2

        //and then, new one proxy
        steg::PLuaDoScript(filePath.c_str());
        //now we have the className luaProxy class in lua global
        //create new table as proxy
        steg::PLuaPushNewTable_J();                         // +1
        int proxyTablePos = steg::PLuaGetTop();
        steg::PLuaPushNil();                                // +1
        steg::PLuaPushFromTable_J(className.c_str());       // +1
        int classTablePos = steg::PLuaGetTop();
        //the class table is on stack top
        steg::PCallLuaFucntionFromTable_J("New", "ttu", NULL, classTablePos, proxyTablePos, this);
        //pop class table
        steg::PLuaPop();                                    // -1
        //pop nil
        steg::PLuaPop();                                    // -1
        //now, the proxy is on stack top
        steg::PLuaSetToGlobal_J(this->name.c_str());        // -1
    }
}

DBG_Status Canvas::InitInScene(steg::Scene* scene)
{
    DBG_Status status = DBG_OK;

    status |= steg::Canvas::InitInScene(scene);

    visible = startVisible;

    return status;
}

}
