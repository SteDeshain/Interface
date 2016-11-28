#ifndef __STEL_H__
#define __STEL_H__

#include <string>
#include "status.h"
#include "lua5.1/lua.h"
#include "lua5.1/lauxlib.h"
#include "lua5.1/lualib.h"

namespace steg
{

extern lua_State* L;

DBG_Status LuaInit();
void LuaQuit();

DBG_Status PLuaDoScript(const char* scriptFile);

DBG_Status PLuaGetGlobal(const char* name, int* value);				// int
DBG_Status PLuaGetGlobal(const char* name, double* value);			// double
DBG_Status PLuaGetGlobal(const char* name, float* value);			// float
DBG_Status PLuaGetGlobal(const char* name, bool* value);			// bool
DBG_Status PLuaGetGlobal(const char* name, std::string* value);		// string
DBG_Status PLuaGetGlobal(const char* name, lua_CFunction* value);	// c function

DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, int* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, double* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, float* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, bool* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, std::string* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, lua_CFunction* value);

//call function already registered in global table LuaFunctions
//DBG_Status PLuaCallRegisteredFunction(const char* luaFunctionName, ...);

}

#endif // __STEL_H__
