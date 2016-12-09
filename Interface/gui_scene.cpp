#include "gui_scene.h"
#include "tools.h"
#include "stdlib.h"
#include "interface_gui.h"
#include "load_comps.h"

namespace interface
{

GuiScene::GuiScene(const char* name, SDL_Renderer* render)
    :InterfaceScene(name, 0, 0, render)
{
}

GuiScene::~GuiScene()
{
}

DBG_Status GuiScene::InitScene()
{
    DBG_Status status = DBG_OK;

    //new luaProxy
    std::string filePath = "scripts/scenes/";
    status |= NewLuaProxy(name, filePath, this);

    //now, load the GameComps registered in script's sources table
    steg::PLuaPushNil();                                            // +1
    steg::PLuaPushFromTable_J(name.c_str());                        // +1
    //now, the proxy table is on stack top
    steg::PLuaPushFromTable_J("sources");                           // +1
    //now, the proxy's sources table is on stack top
    //get the GameComp number
    int compNumber = 0;
    steg::PLuaPushFromTable_J("length");    // +1
    steg::PLuaPop(&compNumber);             // -1

    steg::GameComp* comp = NULL;
    for(int i = 0; i < compNumber; i++)
    {
        comp = PLoadGameCompFromSourcesTable_J(i + 1);
        if(comp)
        {
            (*this) << comp;
        }
    }

    //pop sources table
    steg::PLuaPop();                                                // -1
    //pop proxy table
    steg::PLuaPop();                                                // -1
    //pop nil
    steg::PLuaPop();                                                // -1
    return status;
}

DBG_Status GuiScene::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= InterfaceScene::Update(deltTick);

    //call lua script update
    steg::PCallProxyFunction_J(name.c_str(), "Update", "n", NULL, (double)deltTick);

    return status;
}

DBG_Status GuiScene::UpdatePhysics(Uint32 reminingTick)
{
    DBG_Status status = DBG_OK;

    //do nothing!

    return status;
}

}
