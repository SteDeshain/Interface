#include "gui_scene.h"
#include "tools.h"
#include "stdlib.h"
#include "interface_gui.h"

namespace interface
{

GuiScene::GuiScene(const char* name, SDL_Renderer* render)
    :InterfaceScene(name, 0, 0, render)
{
}

GuiScene::~GuiScene()
{
}

static steg::GameComp* LoadGameCompFromTable(int number)    //stack top is the sources table
{
    lua_pushnumber(steg::L, number);                // +1
    if(!setjmp(steg::StelJmp))
    {
        lua_gettable(steg::L, -2);                  // -1+1
    }
    else
    {
        LUA_LogError("Cannot do lua_gettable!");
    }
    //now, the arguments table is on stack top
    if(!setjmp(steg::StelJmp))
    {
        lua_getfield(steg::L, -1, "class");         // +1
    }
    else
    {
        LUA_LogError("Cannot do lua_gettable!");
    }
    std::string className;
    steg::PLuaPop_J(&className);                    // -1
    if(className == "Canvas")
    {
        std::string mode = "color";
        lua_pushnumber(steg::L, 1);     // +1
        lua_gettable(steg::L, -2);      // -1+1
//        steg::PLuaPop_J(&mode);         // -1
        if(!lua_isnil(steg::L, -1))     // -1
            steg::PLuaPop_J(&mode);
        else
            lua_pop(steg::L, 1);

        std::string scriptName = "";
        lua_pushnumber(steg::L, 2);     // +1
        lua_gettable(steg::L, -2);      // -1+1
        if(!lua_isnil(steg::L, -1))     // -1
            steg::PLuaPop_J(&scriptName);
        else
            lua_pop(steg::L, 1);

        if(mode == "color")
        {
            SDL_Color color;
            int value = 0;
            lua_pushnumber(steg::L, 3); // +1
            lua_gettable(steg::L, -2);  // -1+1
            //color table on stack top
            //r
            lua_pushnumber(steg::L, 1); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            color.r = value;
            //g
            lua_pushnumber(steg::L, 2); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            color.g = value;
            //b
            lua_pushnumber(steg::L, 3); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            color.b = value;
            //pop color table
            lua_pop(steg::L, 1);        // -1

            double transparency = 0.0f;
            lua_pushnumber(steg::L, 4); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&transparency);// -1

            SDL_Rect viewRect;
            lua_pushnumber(steg::L, 5); // +1
            lua_gettable(steg::L, -2);  // -1+1
            //viewRect table on stack top
            //x
            lua_pushnumber(steg::L, 1); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            viewRect.x = value;
            //y
            lua_pushnumber(steg::L, 2); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            viewRect.y = value;
            //w
            lua_pushnumber(steg::L, 3); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            viewRect.w = value;
            //h
            lua_pushnumber(steg::L, 4); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            viewRect.h = value;
            //pop viewRect table
            lua_pop(steg::L, 1);        // -1

            SDL_Point canvasSize;
            lua_pushnumber(steg::L, 6); // +1
            lua_gettable(steg::L, -2);  // -1+1
            //canvasSize table on stack top
            //x
            lua_pushnumber(steg::L, 1); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            canvasSize.x = value;
            //y
            lua_pushnumber(steg::L, 2); // +1
            lua_gettable(steg::L, -2);  // -1+1
            steg::PLuaPop(&value);      // -1
            canvasSize.y = value;
            //pop canvasSize table
            lua_pop(steg::L, 1);        // -1

            void* motherCanvas = NULL;
            lua_pushnumber(steg::L, 7); // +1
            lua_gettable(steg::L, -2);  // -1+1
            if(!lua_isnil(steg::L, -1)) // -1
                steg::PLuaPop(&motherCanvas);
            else
                lua_pop(steg::L, 1);

            steg::GameComp* canvas = NULL;
            canvas = new Canvas(scriptName.c_str(), color, transparency, viewRect, canvasSize, (Canvas*)motherCanvas);
            return canvas;
        }
//        else
//        {
//        }
    }
    else if(className == "GUI")
    {
    }

    //pop arguments table
    lua_pop(steg::L, 1);                            // -1
    return NULL;
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
//    for(int i = 0; i < compNumber; i++)
    for(int i = 0; i < 1; i++)
    {
        comp = LoadGameCompFromTable(i + 1);
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
