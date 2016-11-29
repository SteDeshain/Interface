#include "stel.h"
#include "log.h"

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

DBG_Status PJCallLuaFunction(const char* file, const char* functionPath, const char* argTypes, LuaRets** rets, ...)
{
//    LuaRets* newRets = NULL;
//    if(rets != NULL)
//    {
//        //allocate new LuaRets as results
//        //if any error happens in this function, I MUST remember to delete this object!!!
//        newRets = new LuaRets();
//    }
//    else
//    {
//    }
}

DBG_Status PJLuaPushFromTable(const char* field)    //[-0, +1]
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
        }
    }

    return status;
}

DBG_Status PLuaPeek(double* value)
{
    DBG_Status status = DBG_OK;

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

    double tmp = 0;
    status |= PLuaPeek(&tmp);

    if(status == DBG_OK)
    {
        *value = (int)tmp;
    }

    return status;
}

}
