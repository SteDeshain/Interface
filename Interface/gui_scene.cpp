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

    //register lua functions according to name
    //...
    LegalizeName(name); //the name is legal now
    std::string className = GetClassName(name);

    std::string filePath = "scripts/scenes/";
    filePath += className;
    filePath += ".lua";

    //first, check naming collision
    steg::PLuaPushNil();                            // +1
    steg::PLuaPushFromTable_J(name.c_str());        // +1, actually, push from global
    while(!steg::PLuaTopIsNil())
    {
        NameAddOne(name);
        steg::PLuaPop();                            // -1
        steg::PLuaPushFromTable_J(name.c_str());    // +1
    }
    steg::PLuaPop(2);                               // -2

    //and then, new one proxy
    steg::PLuaDoScript(filePath.c_str());
    //now we have the className luaProxy class in lua global
    //create new table as proxy
    if(!setjmp(steg::StelJmp))
    {
        lua_newtable(steg::L);                                      // +1
    }
    else
    {
        LUA_LogError("Cannot new table! no enough memory");
        status |= DBG_LUA_ERR | DBG_MEM_ERR;
    }
    int proxyTablePos = lua_gettop(steg::L);

    //no need to use PCallProxyFunction here
    //because the code isn't very much
    lua_pushnil(steg::L);                                           // +1
    status |= steg::PLuaPushFromTable_J(className.c_str());         // +1
    int classTablePos = lua_gettop(steg::L);
    //the class table is on stack top
    steg::PCallLuaFucntionFromTable_J("New", "ttu", NULL, classTablePos, proxyTablePos, this);

    //pop class table
    lua_pop(steg::L, 1);                                            // -1
    //pop nil
    lua_pop(steg::L, 1);                                            // -1

    //now, the proxy is on stack top
    if(!setjmp(steg::StelJmp))
    {
        lua_setglobal(steg::L, name.c_str());
    }
    else
    {
        LUA_LogError("Cannot do lua_setglobal!");
    }

    //now, load the GameComps registered in script's sources table
    lua_pushnil(steg::L);                                           // +1
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
    lua_pop(steg::L, 1);                                            // -1
    //pop proxy table
    lua_pop(steg::L, 1);                                            // -1
    //pop nil
    lua_pop(steg::L, 1);                                            // -1
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
