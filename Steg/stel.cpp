#include "stel.h"
#include "log.h"
#include <iostream>

namespace steg
{

jmp_buf StelJmp;
lua_State* L = NULL;

static int cLuaLoadLibs(lua_State* L)
{
	luaL_openlibs(L);

	return 0;
}

int PLuaPanic(lua_State* L)
{
    LUA_LogError(lua_tostring(L, -1));
    longjmp(StelJmp, 1);
}

DBG_Status LuaInit()
{
    DBG_Status status = DBG_OK;

    ENG_LogInfo("Lua module initializing...");

	L = luaL_newstate();
	if(L == NULL)
	{
		ENG_LogError("Lua module initialization failure! Game cannot run!");
		//shut down log module
		LogQuit();
		exit(EXIT_FAILURE);
	}
	else
	{
	    lua_atpanic(L, PLuaPanic);  //it's safe here
		if(lua_cpcall(L, cLuaLoadLibs, NULL))
		{
			//handle error
			const char* errorStr = lua_tostring(L, -1);
			ENG_LogErrors("Lua module cannot open libs: ", errorStr);
			lua_pop(L, 1);
			//shut down log module
            LogQuit();
			lua_close(L);
			exit(EXIT_FAILURE);
		}
		else
		{
			ENG_LogInfo("Lua module initialize successfully!");
		}
	}

    //load lua interface
	status |= PLuaDoScript("./scripts/interface.lua");

    return status;
}

void LuaQuit()
{
    ENG_LogInfo("Lua module shutting down...");
	lua_close(L);
	L = NULL;
	ENG_LogInfo("Lua module shut down successfully!");
}

static const char* (luaLoadErrors[3]) =
{
	[0] = "syntax error during pre-compilation",	//LUA_ERRSYNTAX
	[1] = "memory allocation error",				//LUA_ERRMEM
	[2] = "cannot open/read the script file",		//LUA_ERRFILE
};

static int cLuaDoFile(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	//const char* scriptFile = (char*)lua_topointer(L, 1);  //lua_topointer() "Typically this function is used only for debug information."
	const char* scriptFile = (char*)lua_touserdata(L, 1);
	int err = luaL_loadfile(L, scriptFile);
	if(err)
	{
		//handle errors
		const char* errStr = NULL;
		switch(err)
		{
		case LUA_ERRSYNTAX:
			errStr = luaLoadErrors[0];
			break;
		case LUA_ERRMEM:
			errStr = luaLoadErrors[1];
			break;
		case LUA_ERRFILE:
			errStr = luaLoadErrors[2];
			break;
		default:
			errStr = "Unknown error!";
			break;
		}

		luaL_error(L, "Cannot load script file: %s", errStr);
	}
	else
	{
		//do script
		lua_call(L, 0, 0);
	}

	return 0;	//return values are discarded in cpcall
}

DBG_Status PLuaDoScript(const char* scriptFile)
{
	DBG_Status status = DBG_OK;

	if(scriptFile == NULL)
	{
		return DBG_ARG_ERR;
	}
	else
	{
		if(lua_cpcall(L, cLuaDoFile, (void*)scriptFile))
		{
			//handle error
			const char* errorStr = lua_tostring(L, -1);
			LUA_LogErrors("Lua module cannot do script: ", errorStr);
			lua_pop(L, 1);

			status |= DBG_LUA_ERR;
		}
	}

	return status;
}

DBG_Status PRegisterLuaFunction_J(const char* file, const char* funPath)
{
    DBG_Status status = DBG_OK;

    if(file == NULL || funPath == NULL)
    {
        LUA_LogError("Arguments can't be NULL!");
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    status |= PCallLuaFunctionWithUid_J("readData", "ss", NULL, file, funPath);

    return status;
}

DBG_Status PRegisterLuaFunction_J(const char* file, const char* funPath, const char* funcitonName)
{
    DBG_Status status = DBG_OK;

    if(file == NULL || funPath == NULL || funcitonName == NULL)
    {
        LUA_LogError("Arguments can't be NULL!");
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    status |= PCallLuaFunctionWithUid_J("readData", "ss", NULL, file, funPath);

    lua_pushnil(L);                                                             // +1

    std::string oldFunName = file;
    oldFunName += '&';
    oldFunName += funPath;

    status |= PLuaPushFromTable_J("LuaFunctions");                              // +1
    status |= PLuaPushFromTable_J(oldFunName.c_str());                          // +1

    if(!setjmp(StelJmp))
    {
        lua_setfield(L, -2, funcitonName);                                      // -1
    }
    else
    {
        LUA_LogError("Cannot do lua_setfield!");
        status |= DBG_LUA_ERR;
    }

    lua_pop(L, 1);                                                              // -1
    lua_pop(L, 1);                                                              // -1

    return status;
}

DBG_Status PRegisterCFunction_J(lua_CFunction func, const char* funcitonName)
{
    DBG_Status status = DBG_OK;

    if(funcitonName == NULL)
    {
        LUA_LogError("funcitonName can't be NULL!");
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    lua_pushnil(L);                                                             // +1

    status |= PLuaPushFromTable_J("CFunctions");                                // +1

    if(!setjmp(StelJmp))
    {
        lua_pushcfunction(L, func);                                             // +1
    }
    else
    {
        LUA_LogError("Cannot do lua_pushcfunction!");
        status |= DBG_LUA_ERR;
    }

    if(!setjmp(StelJmp))
    {
        lua_setfield(L, -2, funcitonName);                                      // -1
    }
    else
    {
        LUA_LogError("Cannot do lua_setfield!");
        status |= DBG_LUA_ERR;
    }

    lua_pop(L, 1);                                                              // -1
    lua_pop(L, 1);                                                              // -1

    return status;
}

DBG_Status PLuaReadData_J(const char* file, const char* field, LuaResult** rets)
{
    DBG_Status status = DBG_OK;

    if(file == NULL || field == NULL || rets == NULL)
    {
        LUA_LogError("Arguments can't be NULL!");
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    lua_pushnil(L);                                                     // +1
    status |= PLuaPushFromTable_J("LuaFunctions");                      // +1
    status |= PLuaPushFromTable_J("readData");                          // +1
    status |= PLuaPushFromTable_J("function");                          // +1

    if(!setjmp(StelJmp))
    {
        lua_pushstring(L, file);                                        // +1
    }
    else
    {
        LUA_LogError("Cannot do lua_pushstring!");
    }

    if(!setjmp(StelJmp))
    {
        lua_pushstring(L, field);                                       // +1
    }
    else
    {
        LUA_LogError("Cannot do lua_pushstring!");
    }

    if(lua_pcall(L, 2, 1, 0))                                           // -3
    {
        //hanle error                                       // +1
        if(!setjmp(StelJmp))
            LUA_LogErrors("Error when lua_pcall: ", lua_tostring(L, -1));
        else
            LUA_LogError("Cannot do lua_tostring when logging error!");

        lua_pop(L, 1);                                      // -1

        *rets = NULL;
    }
    else
    {
        //no error                                                      // +1 push the result table
        if(!setjmp(StelJmp))
            lua_getfield(L, -1, "length");      // +1
        else
            LUA_LogError("Cannot do lua_getfield!");

        int resNumber = lua_tonumber(L, -1);
        lua_pop(L, 1);                          // -1

        LuaResult* finRes = new LuaResult(resNumber);
        for(int i = resNumber - 1; i >= 0; i--)
        {
            lua_pushnumber(L, i + 1);           // +1
            lua_gettable(L, -2);                // -1 +1
            finRes->PAddNewResult_J();
            lua_pop(L, 1);                      // -1
        }

        lua_pop(L, 1);                                                  // -1 pop the table

        *rets = finRes;
    }

    //pop function
    lua_pop(L, 1);
    //pop readData
    lua_pop(L, 1);
    //pop LuaFunctions
    lua_pop(L, 1);

    return status;
}

struct typePtrPair
{
	const char* name;
	void* ptr;
	enum luaType
	{
		typeInt,
		typeDouble,
		typeBool,
		typeString,
		typeCFunction,
	} type;
};

static int cLuaGetGlobal(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	typePtrPair* p = (typePtrPair*)lua_touserdata(L, 1);

	lua_getglobal(L, p->name);
	if(lua_isnil(L, -1))
    {
        luaL_error(L, "\"%s\" is not defined yet as a global value!", p->name);
        return 0;
    }
	switch(p->type)
	{
	case typePtrPair::typeInt:		//temp
	case typePtrPair::typeDouble:
		//if(lua_type(L, -1) == LUA_TNUMBER)
		if(lua_isnumber(L, -1))
		{
			double v = lua_tonumber(L, -1);
			if(p->type == typePtrPair::typeInt)
			{
				*((int*)(p->ptr)) = (int)v;
			}
			else	// double
			{
				*((double*)(p->ptr)) = (double)v;
			}
		}
		else
		{
			luaL_error(L, "Global \"%s\" is not a number!", p->name);
		}
		break;
	//case typePtrPair::typeDouble:
	//	break;
	case typePtrPair::typeBool:
		if(lua_isboolean(L, -1))
		{
			*((bool*)(p->ptr)) = lua_toboolean(L, -1);
		}
		else
		{
			luaL_error(L, "Global \"%s\" is not a bool!", p->name);
		}
		break;
	case typePtrPair::typeString:
		if(lua_isstring(L, -1))
		{
			*((std::string*)(p->ptr)) = lua_tostring(L, -1);	//The sequence is copied as the new value for the string.
																//"http://www.cplusplus.com/reference/string/string/operator=/"
		}
		else
		{
			luaL_error(L, "Global \"%s\" is not a string!", p->name);
		}
		break;
	case typePtrPair::typeCFunction:
		if(lua_iscfunction(L, -1))
		{
			*((lua_CFunction*)(p->ptr)) = lua_tocfunction(L, -1);
		}
		else
		{
			luaL_error(L, "Global \"%s\" is not a lua_CFunction!", p->name);
		}
		break;
	default:
		luaL_error(L, "Wrong type!");
		break;
	}
	lua_pop(L, 1);

	return 0;
}

static DBG_Status _pGetGlobal(const char* name, void* value, typePtrPair::luaType t)
{
	DBG_Status status = DBG_OK;

	if(value == NULL || name == NULL)
		return DBG_ARG_ERR | DBG_NULL_PTR;

	typePtrPair* p = new typePtrPair;
	if(p)
	{
		p->name = name;
		p->ptr = value;
		p->type = t;

		if(lua_cpcall(L, cLuaGetGlobal, (void*)p))
		{
			//handle error
			const char* errorStr = lua_tostring(L, -1);
			LUA_LogErrors("Error in lua getglobal: ", errorStr);
			lua_pop(L, 1);

			status |= DBG_LUA_ERR;
		}

		delete p;
	}
	else
	{
		LUA_LogError("Cannot alloc new typePtrPair struct!");
		status |= DBG_MEM_ERR;
	}

	return status;
}

DBG_Status PLuaGetGlobal(const char* name, double* value)
{
	DBG_Status status = DBG_OK;

	status |= _pGetGlobal(name, value, typePtrPair::typeDouble);

	return status;
}

DBG_Status PLuaGetGlobal(const char* name, float* value)
{
	DBG_Status status = DBG_OK;

	double tmp;
	status |= PLuaGetGlobal(name, &tmp);
	*value = (float)tmp;

	return status;
}

DBG_Status PLuaGetGlobal(const char* name, int* value)
{
	DBG_Status status = DBG_OK;

	status |= _pGetGlobal(name, value, typePtrPair::typeInt);

	return status;
}

DBG_Status PLuaGetGlobal(const char* name, bool* value)
{
	DBG_Status status = DBG_OK;

	status |= _pGetGlobal(name, value, typePtrPair::typeBool);

	return status;
}

DBG_Status PLuaGetGlobal(const char* name, std::string* value)
{
	DBG_Status status = DBG_OK;

	status |= _pGetGlobal(name, value, typePtrPair::typeString);

	return status;
}

DBG_Status PLuaGetGlobal(const char* name, lua_CFunction* value)
{
	DBG_Status status = DBG_OK;

	status |= _pGetGlobal(name, value, typePtrPair::typeCFunction);

	return status;
}

DBG_Status PCallLuaFunction_J(const char* file, const char* functionPath, const char* argTypes, LuaResult** rets, ...)
{
    DBG_Status status = DBG_OK;

    if(file == NULL || functionPath == NULL || argTypes == NULL)
    {
        LUA_LogError("function name or argTypes can't be NULL!");
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    std::string uid = file;
    uid += '&';
    uid += functionPath;
    const char* uniqueID = uid.c_str();

    va_list vars;
    va_start(vars, rets);

    //push nil
    lua_pushnil(L);                                                                             // +1

    int argNumber = 0;
    int resNumber = 0;

    PLuaPushFromTable_J("LuaFunctions");                                                        // +1
    PLuaPushFromTable_J(uniqueID);                                                              // +1
    //now the table containing function and argNumber and resNumber is at stack top

    if(lua_isnil(L, -1))
    {
        //no registration
        LUA_LogErrors("Cannot find function, may not be registered! function: ", uniqueID);
        lua_pop(L, 3);
        *rets = NULL;
        return DBG_LUA_ERR;
    }

    //get argNumber
    PLuaPushFromTable_J("argNumber");   // +1
    PLuaPop(&argNumber);                // -1
    //get resNumber
    PLuaPushFromTable_J("resNumber");   // +1
    PLuaPop(&resNumber);                // -1
    //push the function
    PLuaPushFromTable_J("function");                                                            // +1
    //now, the function is at stack top

    //handle and push arguments
    bool atArgTypesEnd = false;
    for(int i = 0; i < argNumber; i++)                                                          // +argNumber
    {
        //once step into this loop, the argNumber is not less than 1 !
        //every step in this loop must push a value!!!

        if(!atArgTypesEnd)
        {
            char current = *(argTypes + i);
            if(current == '\0')
            {
                atArgTypesEnd = true;
                lua_pushnil(L);
            }
            else
            {
                switch(current)
                {
                case 'N':   // nil
                    lua_pushnil(L);
                    break;
                case 'n':   // number
                    lua_pushnumber(L, va_arg(vars, double));
                    break;
                case 's':   // string
                    if(!setjmp(StelJmp))
                        lua_pushstring(L, va_arg(vars, char*));
                    else
                    {
                        LUA_LogError("Cannot push string onto stack!");
                        status |= DBG_LUA_ERR;
                    }
                    break;
                case 'b':   // boolean
                    lua_pushboolean(L, va_arg(vars, bool));
                    break;
                case 'u':   // lightuserdata
                    lua_pushlightuserdata(L, va_arg(vars, void*));
                    break;
                //stack position
                case 't':   //table
                case 'T':   //thread
                case 'f':   //function
                    lua_pushvalue(L, va_arg(vars, int));
                    break;
                default:
                    //push nil at wrong character
                    lua_pushnil(L);
                    break;
                }
            }
        }
        else
        {
            lua_pushnil(L);
        }
    }

    //pcall
    if(lua_pcall(L, argNumber, resNumber, 0))                                                   // -argNumber-1
    {                                           // +1
        //handle error
        std::string  error;
        PLuaPop_J(&error);                      // -1
        LUA_LogErrors("Error when lua_pcall: ", error.c_str());
    }
    else    //pcall successfully                                                                // +resNumber
    {
        //get results
        if(rets != NULL)                                                                        // -resNumber
        {
            //allocate new LuaResult as results
            //if any error happens in this function, I MUST remember to delete this object!!!
            LuaResult* resRets = NULL;
            resRets = new LuaResult(resNumber);
            if(resRets == NULL)
            {
                LUA_LogError("Cannot allocate LuaResult for resRets!");
                lua_pop(L, resNumber);
                *rets = NULL;
            }
            else
            {
                for(int i = 0; i < resNumber; i++)
                {
                    //every step in this loop must pop one element!
                    resRets->PAddNewResult_J();
                    lua_pop(L, 1);
                }
                *rets = resRets;
            }
        }
        else
        {
            lua_pop(L, resNumber);
        }

    }

    //pop function table
    lua_pop(L, 1);                                                                              // -1
    //pop LuaFunctions
    lua_pop(L, 1);                                                                              // -1

    lua_pop(L, 1);  //pop nil                                                                   // -1
                                                                                                // balance
    va_end(vars);

    return status;
}

DBG_Status PCallLuaFunctionWithUid_J(const char* uniqueID, const char* argTypes, LuaResult** rets, ...)
{
    DBG_Status status = DBG_OK;

    va_list vars;
    va_start(vars, rets);

    if(uniqueID == NULL || argTypes == NULL)
    {
        LUA_LogError("function name or argTypes can't be NULL!");
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }
    if(rets)
    {
        *rets = NULL;   // init as NULL first
    }

    //push nil
    lua_pushnil(L);                                                                             // +1

    int argNumber = 0;
    int resNumber = 0;

    PLuaPushFromTable_J("LuaFunctions");                                                        // +1
    PLuaPushFromTable_J(uniqueID);                                                              // +1
    //now the table containing function and argNumber and resNumber is at stack top

    if(lua_isnil(L, -1))
    {
        //no registration
        LUA_LogErrors("Cannot find function, may not be registered! function: ", uniqueID);
        lua_pop(L, 3);
        return DBG_LUA_ERR;
    }

    //get argNumber
    PLuaPushFromTable_J("argNumber");   // +1
    PLuaPop(&argNumber);                // -1
    //get resNumber
    PLuaPushFromTable_J("resNumber");   // +1
    PLuaPop(&resNumber);                // -1
    //push the function
    PLuaPushFromTable_J("function");                                                            // +1
    //now, the function is at stack top

    //handle and push arguments
    bool atArgTypesEnd = false;
    for(int i = 0; i < argNumber; i++)                                                          // +argNumber
    {
        //once step into this loop, the argNumber is not less than 1 !
        //every step in this loop must push a value!!!

        if(!atArgTypesEnd)
        {
            char current = *(argTypes + i);
            if(current == '\0')
            {
                atArgTypesEnd = true;
                lua_pushnil(L);
            }
            else
            {
                switch(current)
                {
                case 'N':   // nil
                    lua_pushnil(L);
                    break;
                case 'n':   // number
                    lua_pushnumber(L, va_arg(vars, double));
                    break;
                case 's':   // string
                    if(!setjmp(StelJmp))
                        lua_pushstring(L, va_arg(vars, char*));
                    else
                    {
                        LUA_LogError("Cannot push string onto stack!");
                        status |= DBG_LUA_ERR;
                    }
                    break;
                case 'b':   // boolean
                    lua_pushboolean(L, va_arg(vars, bool));
                    break;
                case 'u':   // lightuserdata
                    lua_pushlightuserdata(L, va_arg(vars, void*));
                    break;
                //stack position
                case 't':   //table
                case 'T':   //thread
                case 'f':   //function
                    lua_pushvalue(L, va_arg(vars, int));
                    break;
                default:
                    //push nil at wrong character
                    lua_pushnil(L);
                    break;
                }
            }
        }
        else
        {
            lua_pushnil(L);
        }
    }

    //pcall
    if(lua_pcall(L, argNumber, resNumber, 0))                                                   // -argNumber-1
    {                                           // +1
        //handle error
        std::string  error;
        PLuaPop_J(&error);                      // -1
        LUA_LogErrors("Error when lua_pcall: ", error.c_str());
    }
    else    //pcall successfully                                                                // +resNumber
    {
        //get results
        if(rets != NULL)                                                                        // -resNumber
        {
            //allocate new LuaResult as results
            //if any error happens in this function, I MUST remember to delete this object!!!
            LuaResult* resRets = NULL;
            resRets = new LuaResult(resNumber);
            if(resRets == NULL)
            {
                LUA_LogError("Cannot allocate LuaResult for resRets!");
                lua_pop(L, resNumber);
                *rets = NULL;
            }
            else
            {
                for(int i = 0; i < resNumber; i++)
                {
                    //every step in this loop must pop one element!
                    resRets->PAddNewResult_J();
                    lua_pop(L, 1);
                }
                *rets = resRets;
            }
        }
        else
        {
            lua_pop(L, resNumber);
        }

    }

    //pop function table
    lua_pop(L, 1);                                                                              // -1
    //pop LuaFunctions
    lua_pop(L, 1);                                                                              // -1

    lua_pop(L, 1);  //pop nil                                                                   // -1
                                                                                                // balance
    va_end(vars);

    return status;
}

DBG_Status PCallLuaFucntionFromTable_J(const char* functionName, const char* argTypes, LuaResult** rets, ...)
{
    DBG_Status status = DBG_OK;

    va_list vars;
    va_start(vars, rets);

    if(functionName == NULL || argTypes == NULL)
    {
        LUA_LogError("Arguments functionName or argTypes can't be NULL!");
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }
    if(rets)
    {
        *rets = NULL;
    }

    //first, push the function to be called onto the stack
    status |= PLuaPushFromTable_J(functionName);                                            // +1
    if(status != DBG_OK)
        return status;

    //now the table containing function and argNumber and resNumber is at stack top
    //check type
    if(!lua_istable(L, -1))
    {
        //no registration
        LUA_LogErrors("Cannot find function, may be wrong name! when try to call: ", functionName);
        lua_pop(L, 1);
        return DBG_LUA_ERR;
    }

    int argNumber = 0;
    int resNumber = 0;
    //get argNumber
    PLuaPushFromTable_J("argNumber");   // +1
    PLuaPop(&argNumber);                // -1
    //get resNumber
    PLuaPushFromTable_J("resNumber");   // +1
    PLuaPop(&resNumber);                // -1
    //push the function
    PLuaPushFromTable_J("function");                                                        // +1
    //now, the function is at stack top

    //handle and push arguments
    bool atArgTypesEnd = false;
    for(int i = 0; i < argNumber; i++)                                                      // +argNumber
    {
        //once step into this loop, the argNumber is not less than 1 !
        //every step in this loop must push a value!!!

        if(!atArgTypesEnd)
        {
            char current = *(argTypes + i);
            if(current == '\0')
            {
                atArgTypesEnd = true;
                lua_pushnil(L);
            }
            else
            {
                switch(current)
                {
                case 'N':   // nil
                    lua_pushnil(L);
                    break;
                case 'n':   // number
                    lua_pushnumber(L, va_arg(vars, double));
                    break;
                case 's':   // string
                    if(!setjmp(StelJmp))
                        lua_pushstring(L, va_arg(vars, char*));
                    else
                    {
                        LUA_LogError("Cannot push string onto stack!");
                        status |= DBG_LUA_ERR;
                    }
                    break;
                case 'b':   // boolean
                    lua_pushboolean(L, va_arg(vars, bool));
                    break;
                case 'u':   // lightuserdata
                    lua_pushlightuserdata(L, va_arg(vars, void*));
                    break;
                //stack position
                case 't':   //table
                case 'T':   //thread
                case 'f':   //function
                    lua_pushvalue(L, va_arg(vars, int));
                    break;
                default:
                    //push nil at wrong character
                    lua_pushnil(L);
                    break;
                }
            }
        }
        else
        {
            lua_pushnil(L);
        }
    }

    //pcall
    if(lua_pcall(L, argNumber, resNumber, 0))                                               // -argNumber-1
    {                                           // +1
        //handle error
        std::string  error;
        PLuaPop_J(&error);                      // -1
        LUA_LogErrors("Error when lua_pcall: ", error.c_str());
    }
    else    //pcall successfully                                                            // +resNumber
    {
        //get results
        if(rets != NULL)                                                                    // -resNumber
        {
            //allocate new LuaResult as results
            //if any error happens in this function, I MUST remember to delete this object!!!
            LuaResult* resRets = NULL;
            resRets = new LuaResult(resNumber);
            if(resRets == NULL)
            {
                LUA_LogError("Cannot allocate LuaResult for resRets!");
                lua_pop(L, resNumber);
                *rets = NULL;
            }
            else
            {
                for(int i = 0; i < resNumber; i++)
                {
                    //every step in this loop must pop one element!
                    resRets->PAddNewResult_J();
                    lua_pop(L, 1);
                }
                *rets = resRets;
            }
        }
        else
        {
            lua_pop(L, resNumber);
        }

    }

    lua_pop(L, 1);                                                                          // -1

    va_end(vars);
    return status;
}

DBG_Status PCallProxyFunction_J(const char* name, const char* functionName,
                                const char* argTypes, LuaResult** rets, ...)
{
    DBG_Status status = DBG_OK;

    va_list vars;
    va_start(vars, rets);

    if(name == NULL || functionName == NULL || argTypes == NULL)
    {
        LUA_LogError("Arguments name, funcitonName, or argTypes can't be NULL!");
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }
    if(rets)
    {
        *rets = NULL;
    }

    lua_pushnil(L);                                                 // +1
    PLuaPushFromTable_J(name);                                      // +1
    int proxyTablePos = lua_gettop(L);
    //now, the proxy table is on stack top
    //next job is the same as PLuaCallFromTable

    status |= PLuaPushFromTable_J(functionName);                    // +1
    if(status != DBG_OK)
        return status;

    //now the table containing function and argNumber and resNumber is at stack top
    //check type
    if(!lua_istable(L, -1))
    {
        //no registration
        LUA_LogErrors("Cannot find function, may be wrong name! when try to call: ", functionName);
        lua_pop(L, 1);
        return DBG_LUA_ERR;
    }

    int argNumber = 0;
    int resNumber = 0;
    //get argNumber
    PLuaPushFromTable_J("argNumber");   // +1
    PLuaPop(&argNumber);                // -1
    //get resNumber
    PLuaPushFromTable_J("resNumber");   // +1
    PLuaPop(&resNumber);                // -1
    //push the function
    PLuaPushFromTable_J("function");                                                        // +1
    //now, the function is at stack top

    //handle and push arguments
    bool atArgTypesEnd = false;
    for(int i = 0; i < argNumber; i++)                                                      // +argNumber
    {
        //once step into this loop, the argNumber is not less than 1 !
        //every step in this loop must push a value!!!
        if(i == 0)
        {
            //first, push userdata
            //lua_pushlightuserdata(L, userdata);   //wrong! should push proxy table
            lua_pushvalue(L, proxyTablePos);
            continue;
        }

        if(!atArgTypesEnd)
        {
            char current = *(argTypes + i - 1); //attention here, must -1
            if(current == '\0')
            {
                atArgTypesEnd = true;
                lua_pushnil(L);
            }
            else
            {
                switch(current)
                {
                case 'N':   // nil
                    lua_pushnil(L);
                    break;
                case 'n':   // number
                    lua_pushnumber(L, va_arg(vars, double));
                    break;
                case 's':   // string
                    if(!setjmp(StelJmp))
                        lua_pushstring(L, va_arg(vars, char*));
                    else
                    {
                        LUA_LogError("Cannot push string onto stack!");
                        status |= DBG_LUA_ERR;
                    }
                    break;
                case 'b':   // boolean
                    lua_pushboolean(L, va_arg(vars, bool));
                    break;
                case 'u':   // lightuserdata
                    lua_pushlightuserdata(L, va_arg(vars, void*));
                    break;
                //stack position
                case 't':   //table
                case 'T':   //thread
                case 'f':   //function
                    lua_pushvalue(L, va_arg(vars, unsigned int));
                    break;
                default:
                    //push nil at wrong character
                    lua_pushnil(L);
                    break;
                }
            }
        }
        else
        {
            lua_pushnil(L);
        }
    }

    //pcall
    if(lua_pcall(L, argNumber, resNumber, 0))                                               // -argNumber-1
    {                                           // +1
        //handle error
        std::string  error;
        PLuaPop_J(&error);                      // -1
        LUA_LogErrors("Error when lua_pcall: ", error.c_str());
    }
    else    //pcall successfully                                                            // +resNumber
    {
        //get results
        if(rets != NULL)                                                                    // -resNumber
        {
            //allocate new LuaResult as results
            //if any error happens in this function, I MUST remember to delete this object!!!
            LuaResult* resRets = NULL;
            resRets = new LuaResult(resNumber);
            if(resRets == NULL)
            {
                LUA_LogError("Cannot allocate LuaResult for resRets!");
                lua_pop(L, resNumber);
                *rets = NULL;
            }
            else
            {
                for(int i = 0; i < resNumber; i++)
                {
                    //every step in this loop must pop one element!
                    resRets->PAddNewResult_J();
                    lua_pop(L, 1);
                }
                *rets = resRets;
            }
        }
        else
        {
            lua_pop(L, resNumber);
        }

    }

    //pop function table
    lua_pop(L, 1);                                                  // -1
    //pop proxy table
    lua_pop(L, 1);                                                  // -1
    //pop nil
    lua_pop(L, 1);                                                  // -1

    va_end(vars);
    return status;
}

DBG_Status PLuaPushFromTable_J(const char* field)    //[-0, +1]
{
    DBG_Status status = DBG_OK;

    bool readFromGlobal = false;
    if(lua_gettop(L) <= 0)
    {
        readFromGlobal = true;
    }
    else
    {
        if(!lua_istable(L, -1))
        {
            readFromGlobal = true;
        }
    }

    if(readFromGlobal)
    {
        if(!setjmp(StelJmp))    // try
        {
            lua_getglobal(L, field);    //[-0, +1, e]
        }
        else                    // catch
        {
            //necessary?
            LUA_LogError("thrown error catched, happenning in PJLuaPushFromTable, error message is at the previous line!");
            status |= DBG_LUA_ERR;
        }
    }
    else
    {
        if(!setjmp(StelJmp))    // try
        {
            lua_getfield(L, -1, field);     //[-0, +1, e]
        }
        else                    // catch
        {
            //necessary?
            LUA_LogError("thrown error catched, happenning in PJLuaPushFromTable, error message is at the previous line!");
            status |= DBG_LUA_ERR;
        }
    }

    return status;
}

DBG_Status PLuaPeek(double* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return DBG_NULL_PTR;

    if(lua_gettop(L) <= 0)
    {
        LUA_LogError("Lua stack is empty, cannot peek!");
        return DBG_LUA_ERR;
    }

    if(!lua_isnumber(L, -1))
    {
        LUA_LogError("Lua stack top is not a number, can't assign it to a double!");
        return DBG_LUA_ERR;
    }

    *value = lua_tonumber(L, -1);

    return status;
}

DBG_Status PLuaPeek(float* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return DBG_NULL_PTR;

    double tmp = 0;
    status |= PLuaPeek(&tmp);

    if(status == DBG_OK)
    {
        *value = (float)tmp;
    }

    return status;
}

DBG_Status PLuaPeek(int* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return DBG_NULL_PTR;

    double tmp = 0;
    status |= PLuaPeek(&tmp);

    if(status == DBG_OK)
    {
        *value = (int)tmp;
    }

    return status;
}

DBG_Status PLuaPeek(bool* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return DBG_NULL_PTR;

    if(lua_gettop(L) <= 0)
    {
        LUA_LogError("Lua stack is empty, cannot peek!");
        return DBG_LUA_ERR;
    }

    if(!lua_isboolean(L, -1))
    {
        LUA_LogError("Lua stack top is not a boolean, can't assign it to a bool!");
        return DBG_LUA_ERR;
    }

    *value = lua_toboolean(L, -1);

    return status;
}

DBG_Status PLuaPeek_J(std::string* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return DBG_NULL_PTR;

    if(lua_gettop(L) <= 0)
    {
        LUA_LogError("Lua stack is empty, cannot peek!");
        return DBG_LUA_ERR;
    }

    if(lua_type(L, -1) != LUA_TSTRING)  // using lua_isstring will treat number as a string too
    {
        LUA_LogError("Lua stack top is not a string, can't assign it to a std::string!");
        return DBG_LUA_ERR;
    }

    if(lua_isnil(L, -1))
    {
        return DBG_LUA_ERR;
    }

    if(!setjmp(StelJmp))    //try
    {
        *value = lua_tostring(L, -1);   //make a copy of string to value
    }
    else                    //catch
    {
        LUA_LogError("Error thrown in PLuaPeek_J(std::string* value) when calling lua_tostring");
        status |= DBG_LUA_ERR;
    }

    return status;
}

DBG_Status PLuaPeek(lua_CFunction* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return DBG_NULL_PTR;

    if(lua_gettop(L) <= 0)
    {
        LUA_LogError("Lua stack is empty, cannot peek!");
        return DBG_LUA_ERR;
    }

    if(!lua_iscfunction(L, -1))
    {
        LUA_LogError("Lua stack top is not a cfunction, can't assign it to a lua_CFunction!");
        return DBG_LUA_ERR;
    }

    *value = lua_tocfunction(L, -1);

    return status;
}

DBG_Status PLuaPeek(void** value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return DBG_ARG_ERR | DBG_NULL_PTR;

    if(lua_gettop(L) <= 0)
    {
        LUA_LogError("Lua stack is empty, cannot peek!");
        return DBG_LUA_ERR;
    }

    if(!lua_isuserdata(L, -1))
    {
        LUA_LogError("Lua stack top is not a userdata, can't assign it to a void*!");
        return DBG_LUA_ERR;
    }

    *value = lua_touserdata(L, -1);

    return status;
}

DBG_Status PLuaPop()
{
    lua_pop(L, 1);
    return DBG_OK;
}

DBG_Status PLuaPop(double* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return status;

    status |= PLuaPeek(value);
    lua_pop(L, 1);

    return status;
}

DBG_Status PLuaPop(float* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return status;

    status |= PLuaPeek(value);
    lua_pop(L, 1);

    return status;
}

DBG_Status PLuaPop(int* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return status;

    status |= PLuaPeek(value);
    lua_pop(L, 1);

    return status;
}

DBG_Status PLuaPop(bool* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return status;

    status |= PLuaPeek(value);
    lua_pop(L, 1);

    return status;
}

DBG_Status PLuaPop_J(std::string* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return status;

    status |= PLuaPeek_J(value);
    lua_pop(L, 1);

    return status;
}

DBG_Status PLuaPop(lua_CFunction* value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return status;

    status |= PLuaPeek(value);
    lua_pop(L, 1);

    return status;
}

DBG_Status PLuaPop(void** value)
{
    DBG_Status status = DBG_OK;

    if(value == NULL)
        return status;

    status |= PLuaPeek(value);
    lua_pop(L, 1);

    return status;
}

LuaResult::LuaResult(unsigned int n)
{
    resultNumber = n;
    if(n != 0)
    {
        ptr = new Result*[n];
        if(ptr == NULL)
        {
            LUA_LogError("Cannot allocate Result* array for LuaResult when attempt doing PCallLuaFunction");
        }
        else
        {
            for(int i = 0; i < n; i++)
            {
                ptr[i] = NULL;
            }
        }
    }
}

LuaResult::~LuaResult()
{
    if(ptr)
    {
        //delete the ptr CAREFULLY!!!
        for(int i = 0; i < resultNumber; i++)
        {
            //delete ptr[i] as a struct Result
            if(ptr[i])
            {
                //delete ptr[i]
                if(ptr[i]->value)
                {
                    if(ptr[i]->type != 'u') //don't delete userdata
                    {
                        //delete ptr[i]->value
                        delete ptr[i]->value;
                        ptr[i]->value = NULL;
                    }
                }
                delete ptr[i];
                ptr[i] = NULL;
            }
        }
        //delete[] ptr
        delete[] ptr;
        ptr = NULL;
    }
}

int LuaResult::GetResultNumber()
{
    return resultNumber;
}

DBG_Status LuaResult::PAddNewResult_J()
{
    DBG_Status status = DBG_OK;

    if(!ptr)    // 0 resultNumber
        return DBG_OK;

    if(lua_gettop(L) <= 0)
    {
        LUA_LogError("The lua stack should not be empty!");
        return DBG_LUA_ERR;
    }

    if(currentIndex >= resultNumber)
        return DBG_OUT_OF_RANGE | DBG_LUA_ERR;

    int i = resultNumber - currentIndex - 1;
    ptr[i] = new Result;
    if(ptr[i] == NULL)
    {
        LUA_LogError("Cannot allocate struct Result for each element in LuaResult!");
        return DBG_MEM_ERR;
    }
    else
    {
        //default set as nil
        ptr[i]->type = 'N';
        ptr[i]->value = NULL;
    }

    switch(lua_type(L, -1))
    {
    case LUA_TNIL:
        //do nothing, because ptr[currentIndex] was already set to nil as default
        break;
    case LUA_TNUMBER:
        ptr[i]->type = 'n';
        ptr[i]->value = new double;
        if(ptr[i]->value == NULL)
        {
            LUA_LogError("Cannot allocate double for struct Result!");
        }
        else
        {
            *((double*)(ptr[i]->value)) = lua_tonumber(L, -1);
        }
        break;
    case LUA_TBOOLEAN:
        ptr[i]->type = 'b';
        ptr[i]->value = new bool;
        if(ptr[i]->value == NULL)
        {
            LUA_LogError("Cannot allocate bool for struct Result!");
        }
        else
        {
            *((bool*)(ptr[i]->value)) = lua_toboolean(L, -1);
        }
        break;
    case LUA_TSTRING:
        ptr[i]->type = 's';
        ptr[i]->value = new std::string;
        if(ptr[i]->value == NULL)
        {
            LUA_LogError("Cannot allocate std::string for struct Result!");
        }
        else
        {
            if(!setjmp(StelJmp))    //try
            {
                *((std::string*)(ptr[i]->value)) = lua_tostring(L, -1);
            }
            else                    //catch
            {
                LUA_LogError("Lua cannot tostring!");
            }
        }
        break;
    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
        ptr[i]->type = 'u';
        ptr[i]->value = lua_touserdata(L, -1);   //no need to new
        break;

    case LUA_TTABLE:
        ptr[i]->type = 't';
        break;
    case LUA_TFUNCTION:
        ptr[i]->type = 'f';
        break;
    case LUA_TTHREAD:
        ptr[i]->type = 'T';
        break;
    case LUA_TNONE:
    default:
        LUA_LogError("Something unexpected happened!");
        break;
    }

    currentIndex++;

    return status;
}

LuaResult::Result* LuaResult::operator[](unsigned int i)
{
    if(i >= resultNumber)
    {
        LUA_LogError("Too big i to index LuaResult!");
        return NULL;
    }

    return ptr[i];
}

static void printResult(LuaResult::Result* r)
{
    switch(r->type)
    {
    case 'N':
        std::cout << "nil";
        break;
    case 'n':
        std::cout << "number: " << *((double*)(r->value));
        break;
    case 's':
        std::cout << "string: " << *((std::string*)(r->value));
        break;
    case 'b':
        std::cout << "boolean: " << *((bool*)(r->value));
        break;
    case 't':
        std::cout << "table";
        break;
    case 'f':
        std::cout << "function";
        break;
    case 'T':
        std::cout << "thread";
        break;
    case 'u':
        std::cout << "userdata: " << r->value;
        break;
    default:
        break;
    }
}

void LuaResult::PrintToStanderdOut()
{
    std::cout << "\nLuaResult:" << std::endl;
    for(int i = 0; i < resultNumber; i++)
    {
        std::cout << "    [" << i + 1 << "]";
        printResult(ptr[i]);
        std::cout << std::endl;
    }
}

}
