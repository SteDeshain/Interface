// c functions ready to register in lua module

#ifndef __C_API_H__
#define __C_API_H__

#include "lua5.1/lua.h"
#include "lua5.1/lauxlib.h"
#include "lua5.1/lualib.h"
#include "status.h"

namespace steg
{

DBG_Status RegisterFunctions();

int Test(lua_State* L);

int ShowCanvas(lua_State* L);
int HideCanvas(lua_State* L);

}

#endif // __C_API_H__
