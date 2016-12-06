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
extern int stackSize;

DBG_Status LuaInit();
void LuaQuit();

// P: for complete protection mode
// J: for long jump fake protection mode

class LuaResult;  //pre-declaration

//must be called in pair
//void StoreLuaStack();
//void ResumeLuaStack();

DBG_Status PLuaDoScript(const char* scriptFile);

DBG_Status PRegisterLuaFunction_J(const char* file, const char* funPath, const char* funcitonName);   //register as a new name
DBG_Status PRegisterLuaFunction_J(const char* file, const char* funPath);
DBG_Status PRegisterCFunction_J(lua_CFunction func, const char* funcitonName);

DBG_Status PLuaReadData_J(const char* file, const char* field, LuaResult** rets);

DBG_Status PLuaGetGlobal(const char* name, double* value);			// double
DBG_Status PLuaGetGlobal(const char* name, int* value);				// int
DBG_Status PLuaGetGlobal(const char* name, float* value);			// float
DBG_Status PLuaGetGlobal(const char* name, bool* value);			// bool
DBG_Status PLuaGetGlobal(const char* name, std::string* value);		// string
DBG_Status PLuaGetGlobal(const char* name, lua_CFunction* value);	// c function

// before call this function, first declare a LuaResult** pointer to pass to this function
// so that it can store the results to this pointer
// but the user must delete the LuaResult object at this address after using it
// if rets == NULL, then the function will not do anything about getting lua function's results
// ***
// the argTypes is a string may containing character like:
// 's'     'n'     'b'      'u'       'N'
// string, number, boolean, userdata, nil
// 't'    'T'     'f'           these three types use an positive integer to show their position in stack
// table, thread, function
// other characters in this string will cause pushing a nil
// the final arguments number will be the number stored in LuaFunction table
// ***
// as for getting results, lua_pcall will push results by the number of nresults
// and when PCallLuaFunction_J is getting results, it'll pop elemnts by number of nresults, the same number
// do it's safe to operate the stack when getting results
// ***
// example: ..., "sNn", "a string", 100);    //push "a string", nil, 100 (if argNumber == 3)
//               "", 1, 2, 3);               //push nil, nil (if argNumber == 2)
DBG_Status PCallLuaFunctionWithUid_J(const char* uniqueID, const char* argTypes, LuaResult** rets, ...);
DBG_Status PCallLuaFunction_J(const char* file, const char* functionPath, const char* argTypes, LuaResult** rets, ...);
//if stack top is a table, call function with funcitonName string in table
//otherwise, call function from _G
DBG_Status PCallLuaFucntionFromTable_J(const char* functionName, const char* argTypes, LuaResult** rets, ...);
// when calling this function, you don't need to add 'u' at begining of "argTypes", and "this" at begining of "..."
DBG_Status PCallProxyFunction_J(const char* name, const char* functionName,
                                const char* argTypes, LuaResult** rets, ...);

//******************************// Stack Operation //******************************//
// if at the stack top is a table, then get the field from the top table, and push it onto stack
// else, get the field from the _G table, and push it onto stack
// if can't find field or any error happens, push nil onto stack
DBG_Status PLuaPushFromTable_J(const char* field);    // [-0, +1]

DBG_Status PLuaPeek(double* value);                 // [-0, +0]
DBG_Status PLuaPeek(int* value);
DBG_Status PLuaPeek(float* value);
DBG_Status PLuaPeek(bool* value);
DBG_Status PLuaPeek_J(std::string* value);
DBG_Status PLuaPeek(lua_CFunction* value);
DBG_Status PLuaPeek(void** value);

DBG_Status PLuaPop();                               // [-1, +0] pop one element from stack
DBG_Status PLuaPop(double* value);                  // [-1, +0] (-0 only when the stack is empty)
DBG_Status PLuaPop(int* value);
DBG_Status PLuaPop(float* value);
DBG_Status PLuaPop(bool* value);
DBG_Status PLuaPop_J(std::string* value);
DBG_Status PLuaPop(lua_CFunction* value);
DBG_Status PLuaPop(void** value);


// lua may return: nil, number, string, boolean, table, function, thread, userdata (total: 8)
//                 'N'  'n'     's'     'b'      't'    'f'       'T'     'u'
// we can't express table, function(lua function), thread in c++
// so there are 5 left: nil, number, string, boolean, userdata
class LuaResult
{

public:
    struct Result
    {
        char type;
        void* value;
    };

public:
    LuaResult(unsigned int n);
    ~LuaResult();

    int GetResultNumber();

    DBG_Status PAddNewResult_J();   //use the lua stack top value to add(make a copy if necessary)
                                    //if the currentIndex is beyond resultNumber, it'll do nothing and return, and log an error

    Result* operator[](unsigned int i); //user can't delete the pointer returned by []

    void PrintToStanderdOut();

private:
    unsigned int resultNumber = 0;
    unsigned int currentIndex = 0;  //inner use index the current element in AddNewResult
    Result** ptr = NULL;    //array of Result*, each element point to a struct Result

};

}

#endif // __STEL_H__
