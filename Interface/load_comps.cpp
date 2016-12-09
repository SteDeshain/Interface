#include "load_comps.h"
#include "stel.h"
#include "interface_game.h"
#include "tools.h"

namespace interface
{

//assuming the arguments table is on stack top
static Canvas* PLoadCanvas_J()
{
    Canvas* comp = NULL;

    int tempInteger = 0;

    std::string mode = "color";
    std::string scriptName = "";
    double transparency = 1.0f;
    SDL_Rect viewRect = {0, 0, 0, 0};
    SDL_Point canvasSize = {0, 0};
    void* motherCanvas = NULL;
    bool startVisible = true;

    //mode
    steg::PLuaPushFromTable_J(1);   // +1
    steg::PLuaPop_J(&mode);         // -1

    //name
    steg::PLuaPushFromTable_J(2);   // +1
    //if top is nil, the PLuaPop will not change the scriptName, so it's safety
    steg::PLuaPop_J(&scriptName);   // -1

    //transparency
    steg::PLuaPushFromTable_J(4);   // +1
    steg::PLuaPop(&transparency);   // -1

    //viewRect
    steg::PLuaPushFromTable_J(5);   // +1
    //now, the viewRect table is on stack top
    PGetSDLRectFromTopTable_J(&viewRect);
    steg::PLuaPop();                // -1

    //canvasSize
    steg::PLuaPushFromTable_J(6);   // +1
    PGetSDLPointFromTopTable_J(&canvasSize);
    steg::PLuaPop();                // -1

    //motherCanvas
    steg::PLuaPushFromTable_J(7);   // +1
    steg::PLuaPop(&motherCanvas);   // -1

    //startVisible
    steg::PLuaPushFromTable_J(8);   // +1
    steg::PLuaPop(&startVisible);   // -1

    if(mode == "color")
    {
        SDL_Color color = {0, 0, 0, 0};
        steg::PLuaPushFromTable_J(3);       // +1
        PGetSDLColorFromTopTable_J(&color);
        steg::PLuaPop();                    // -1

        comp = new Canvas(scriptName.c_str(), color, transparency, viewRect, canvasSize, (Canvas*)motherCanvas, startVisible);
    }
    else
    {
        std::string* imgFile = new std::string("");
        InterfaceGame::usedStrings.push_back(imgFile);
        steg::PLuaPushFromTable_J(3);       // +1
        steg::PLuaPop_J(imgFile);          // -1

        comp = new Canvas(scriptName.c_str(), imgFile->c_str(), transparency, viewRect, canvasSize,
                          (Canvas*)motherCanvas, startVisible);
    }

    return comp;
}

static GameComp* PLoadGUI_J()
{
    GameComp* comp = NULL;

    return comp;
}

GameComp* PLoadGameCompFromSourcesTable_J(int number)
{
    GameComp* comp = NULL;

    std::string className = "";
    steg::PLuaPushFromTable_J(number);              // +1
    //now, the arguments table is on stack top
    steg::PLuaPushFromTable_J("class");             // +1
    steg::PLuaPop_J(&className);                    // -1
    if(className == "Canvas")
    {
        comp = (GameComp*)PLoadCanvas_J();
    }
    else if(className == "GUI")
    {
        comp = (GameComp*)PLoadGUI_J();
    }

    steg::PLuaPop();                                // -1

    return comp;
}

DBG_Status PGetSDLRectFromTopTable_J(SDL_Rect* rect)
{
    DBG_Status status = DBG_OK;

    if(rect == NULL)
    {
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    int temp = 0;
    status |= steg::PLuaPushFromTable_J(1);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    rect->x = temp;

    status |= steg::PLuaPushFromTable_J(2);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    rect->y = temp;

    status |= steg::PLuaPushFromTable_J(3);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    rect->w = temp;

    status |= steg::PLuaPushFromTable_J(4);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    rect->h = temp;

    return status;
}

DBG_Status PGetSDLPointFromTopTable_J(SDL_Point* point)
{
    DBG_Status status = DBG_OK;

    if(point == NULL)
    {
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    int temp = 0;
    status |= steg::PLuaPushFromTable_J(1);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    point->x = temp;

    status |= steg::PLuaPushFromTable_J(2);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    point->y = temp;

    return status;
}

DBG_Status PGetSDLColorFromTopTable_J(SDL_Color* color)
{
    DBG_Status status = DBG_OK;

    if(color == NULL)
    {
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    int temp = 0;
    status |= steg::PLuaPushFromTable_J(1);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    color->r = temp;

    status |= steg::PLuaPushFromTable_J(2);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    color->g = temp;

    status |= steg::PLuaPushFromTable_J(3);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    color->b = temp;

    status |= steg::PLuaPushFromTable_J(4);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    color->a = temp;

    return status;
}

DBG_Status NewLuaProxy(std::string& name, std::string& filePath, void* ud)
{
    DBG_Status status = DBG_OK;
    if(ud == NULL)
        return DBG_ARG_ERR | DBG_NULL_PTR;

    LegalizeName(name);
    std::string className = GetClassName(name);
    if(filePath.empty())
    {
        filePath = "scripts/";
    }
    filePath += className;
    filePath += ".lua";

    //first, check naming collision
    steg::PLuaPushNil();                                            // +1
    status |= steg::PLuaPushFromTable_J(name.c_str());              // +1, actually, push from global
    while(!steg::PLuaTopIsNil())
    {
        NameAddOne(name);
        steg::PLuaPop();                            // -1
        steg::PLuaPushFromTable_J(name.c_str());    // +1
    }
    steg::PLuaPop(2);                                               // -2

    //and then, new one proxy
    status |= steg::PLuaDoScript(filePath.c_str());
    //now we have the className luaProxy class in lua global
    //create new table as proxy
    status |= steg::PLuaPushNewTable_J();                           // +1
    int proxyTablePos = steg::PLuaGetTop();

    //no need to use PCallProxyFunction here
    //because the code isn't very much
    steg::PLuaPushNil();                                            // +1
    status |= steg::PLuaPushFromTable_J(className.c_str());         // +1
    int classTablePos = steg::PLuaGetTop();
    //the class table is on stack top
//    PrintStackTop();
    steg::PCallLuaFucntionFromTable_J("New", "ttu", NULL, classTablePos, proxyTablePos, ud);

    //pop class table
    steg::PLuaPop();                                                // -1
    //pop nil
    steg::PLuaPop();                                                // -1

    //now, the proxy is on stack top
    //register it to global
    status |= steg::PLuaSetToGlobal_J(name.c_str());                // -1

    return status;
}

}

