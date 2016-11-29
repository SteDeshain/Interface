#ifndef __STEL_H__
#define __STEL_H__

#include <csetjmp>
#include <string>
#include <cstdarg>
#include "status.h"
#include "lua5.1/lua.h"
#include "lua5.1/lauxlib.h"
#include "lua5.1/lualib.h"

namespace steg
{

extern jmp_buf StelJmp;
extern lua_State* L;

DBG_Status LuaInit();
void LuaQuit();

// P: for complete protection mode
DBG_Status PLuaDoScript(const char* scriptFile);

DBG_Status PLuaGetGlobal(const char* name, double* value);			// double
DBG_Status PLuaGetGlobal(const char* name, int* value);				// int
DBG_Status PLuaGetGlobal(const char* name, float* value);			// float
DBG_Status PLuaGetGlobal(const char* name, bool* value);			// bool
DBG_Status PLuaGetGlobal(const char* name, std::string* value);		// string
DBG_Status PLuaGetGlobal(const char* name, lua_CFunction* value);	// c function

/**
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, int* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, double* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, float* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, bool* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, std::string* value);
DBG_Status PLuaGetFromGlobalTable(const char* table, const char* field, lua_CFunction* value);
/**/

// J: for long jump fake protection mode

class LuaRets;  //pre-declaration

// before call this function, first declare a LuaRets* pointer to pass to this function
// so that it can restore the results to this pointer
// but the caller must delete the LuaRets object at this address after using it
// if rets == NULL, then the function will not do anything about getting lua function's results
// ***
// the argTypes is a string may containing character like:
// 's' for strnig; 'n' for number; 'b' for boolean; 'u' for userdata;
// other characters in this string will be ignored
// the final arguments number will be the number stored in LuaFunction table
DBG_Status PJCallLuaFunction(const char* file, const char* functionPath, const char* argTypes, LuaRets** rets, ...);
DBG_Status PJCallLuaFunctionWithUid(const char* uniqueID, const char* argTypes, LuaRets** rets, ...);

//******************************// Stack Operation //******************************//
// if at the stack top is a table, then get the field from the top table, and push it onto stack
// else, get the field from the _G table, and push it onto stack
// if can't find field or any error happens, push nil onto stack
DBG_Status PJLuaPushFromTable(const char* field);    // [-0, +1]

DBG_Status PJLuaPop(double* value);                  // [-1, +0] (-0 only when the stack is empty)
DBG_Status PJLuaPop(int* value);
DBG_Status PJLuaPop(float* value);
DBG_Status PJLuaPop(bool* value);
DBG_Status PJLuaPop(std::string* value);
DBG_Status PJLuaPop(lua_CFunction* value);

DBG_Status PLuaPeek(double* value);                 // [-0, +0]
DBG_Status PLuaPeek(int* value);
DBG_Status PLuaPeek(float* value);
DBG_Status PLuaPeek(bool* value);
DBG_Status PJLuaPeek(std::string* value);
DBG_Status PLuaPeek(lua_CFunction* value);


// lua may return: nil, number, string, boolean, table, function, thread, userdata (total: 8)
// we can't express table, function(lua function), thread
// so there are 5 left
class LuaRets
{

public:
    LuaRets();
    ~LuaRets();

    int GetResultNumber();

private:
    int resultNumber = 0;

};

}

#endif // __STEL_H__
