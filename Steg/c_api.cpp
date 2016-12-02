#include "c_api.h"

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
