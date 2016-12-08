#include "load_comps.h"
#include "stel.h"
#include "interface_game.h"

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
        comp = PLoadGUI_J();
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

}

