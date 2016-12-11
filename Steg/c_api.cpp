#include "c_api.h"
#include "stel.h"
#include "event_handler.h"
#include "gui.h"

namespace steg
{

DBG_Status RegisterFunctions()
{
    DBG_Status status = DBG_OK;

    status |= PRegisterCFunction_J(ShowCanvas, "showCanvas");
    status |= PRegisterCFunction_J(HideCanvas, "hideCanvas");

    return status;
}

int Test(lua_State* L)
{
    luaL_checknumber(L, -1);
    double a = lua_tonumber(L, -1);
    lua_pop(L, 1);

    double res = a * 2;
    lua_pushnumber(L, res);
    lua_pushnumber(L, res * 2);

    return 2;
}

int ShowCanvas(lua_State* L)
{
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    //temp
    void* canvas = lua_touserdata(L, -1);
//    Canvas* c = dynamic_cast
    lua_pop(L, 1);

    PushShowCanvasEvent(canvas);

    return 0;   //0 result
}

int HideCanvas(lua_State* L)
{
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    //temp
    void* canvas = lua_touserdata(L, -1);
    lua_pop(L, 1);

    PushHideCanvasEvent(canvas);

    return 0;   //0 result
}

}
