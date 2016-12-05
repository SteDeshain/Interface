#include "gui_scene.h"
#include "tools.h"
#include "stdlib.h"

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
//    steg::StoreLuaStack();
    if(!setjmp(steg::StelJmp))
    {
        lua_getglobal(steg::L, name.c_str());           // +1
        while(!lua_isnil(steg::L, -1))
        {
            NameAddOne(name);
            lua_pop(steg::L, 1);                    // -1
            lua_getglobal(steg::L, name.c_str());   // +1
        }
        lua_pop(steg::L, 1);                            // -1
    }
    else
    {
        LUA_LogError("Cannot do lua_getglobal!");
        //do not check, assuming no collision
    }
//    steg::ResumeLuaStack();

    //and then, new one proxy
    //...
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
    //
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

    //tmp
//    luaL_dostring(steg::L,
//                  "print(welcomeScene_1)");
//    std::string luaTasks = "print(";
//    luaTasks += name;
//    luaTasks += ".ud)";
//    luaL_dostring(steg::L, luaTasks.c_str());
//    std::cout << this << std::endl;

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
