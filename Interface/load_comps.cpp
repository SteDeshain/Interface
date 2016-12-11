#include "load_comps.h"
#include "stel.h"
#include "interface_game.h"
#include "tools.h"
#include <vector>

namespace interface
{

//assuming the arguments table is on stack top
static Canvas* PLoadCanvas_J(const std::string& sceneName)
{
    Canvas* comp = NULL;

//    int tempInteger = 0;

    std::string mode = "color";
    std::string name = "";
    double transparency = 1.0f;
    SDL_Rect viewRect = {0, 0, 0, 0};
    SDL_Point canvasSize = {0, 0};
    void* motherCanvas = NULL;
    bool startVisible = true;

    //name
    steg::PLuaPushFromTable_J("name");   // +1
    //if top is nil, the PLuaPop will not change the name, so it's safety
    steg::PLuaPop_J(&name);   // -1

    //mode
    steg::PLuaPushFromTable_J(1);   // +1
    steg::PLuaPop_J(&mode);         // -1

    //transparency
    steg::PLuaPushFromTable_J(3);   // +1
    steg::PLuaPop(&transparency);   // -1

    //viewRect
    steg::PLuaPushFromTable_J(4);   // +1
    //now, the viewRect table is on stack top
    PGetSDLRectFromTopTable_J(&viewRect);
    steg::PLuaPop();                // -1

    //canvasSize
    steg::PLuaPushFromTable_J(5);   // +1
    PGetSDLPointFromTopTable_J(&canvasSize);
    steg::PLuaPop();                // -1

    //motherCanvas
    std::string findPath = "";
    steg::PLuaPushFromTable_J(6);   // +1
    steg::PLuaPop(&motherCanvas);   // -1

    //startVisible
    steg::PLuaPushFromTable_J(7);   // +1
    steg::PLuaPop(&startVisible);   // -1

    if(mode == "color")
    {
        SDL_Color color = {0, 0, 0, 0};
        steg::PLuaPushFromTable_J(2);       // +1
        PGetSDLColorFromTopTable_J(&color);
        steg::PLuaPop();                    // -1

        comp = new Canvas(name.c_str(), color, transparency, viewRect, canvasSize, (Canvas*)motherCanvas, startVisible);
    }
    else
    {
        std::string* imgFile = new std::string("");
        InterfaceGame::usedStrings.push_back(imgFile);
        steg::PLuaPushFromTable_J(2);       // +1
        steg::PLuaPop_J(imgFile);           // -1

        comp = new Canvas(name.c_str(), imgFile->c_str(), transparency, viewRect, canvasSize,
                          (Canvas*)motherCanvas, startVisible);
    }

    return comp;
}

static GUI* PLoadGUI_J(const std::string& sceneName)
{
    GUI* comp = NULL;

//    int tempInteger = 0;

    std::string name = "";
    std::string mode = "color";
    int x = 0;
    int y = 0;
    int drawIndex = 0;
    void* motherCanvas = NULL;

    //name
    steg::PLuaPushFromTable_J("name");   // +1
    //if top is nil, the PLuaPop will not change the name, so it's safety
    steg::PLuaPop_J(&name);             // -1

    //mode
    steg::PLuaPushFromTable_J(1);   // +1
    steg::PLuaPop_J(&mode);         // -1

    //x, y
    steg::PLuaPushFromTable_J(2);
    steg::PLuaPop(&x);
    steg::PLuaPushFromTable_J(3);
    steg::PLuaPop(&y);

    //drawIndex
    steg::PLuaPushFromTable_J(4);
    steg::PLuaPop(&drawIndex);

    //motherCanvas
    std::string findPath = "";
    steg::PLuaPushFromTable_J(5);
    steg::PLuaPop_J(&findPath);
    PushFromFindPath(findPath, sceneName);      // +1
    steg::PLuaPop(&motherCanvas);               // -1

    //tmp:
//    luaL_dostring(steg::L,
//                  "print(welcomeScene_1.sources)");
//    luaL_dostring(steg::L,
//                  "print(unpack(welcomeScene_1.sources))");
//    luaL_dostring(steg::L,
//                  "print(welcomeScene_1.sources.logoCanv_1)");
//    luaL_dostring(steg::L,
//                  "print(welcomeScene_1.sources.logoCanv_1.ud)");

    if(mode == "color")
    {
        //picSize
        SDL_Point picSize = {0, 0};
        steg::PLuaPushFromTable_J(6);
        PGetSDLPointFromTopTable_J(&picSize);
        steg::PLuaPop();

        //color
        SDL_Color color = {0, 0, 0, 0};
        steg::PLuaPushFromTable_J(7);
        PGetSDLColorFromTopTable_J(&color);
        steg::PLuaPop();

        //transparency
        double transparency = 1.0f;
        steg::PLuaPushFromTable_J(8);
        steg::PLuaPop(&transparency);

        comp = new GUI(name.c_str(), x, y, drawIndex, picSize, color, transparency, (Canvas*)motherCanvas);
    }
    else    //picture
    {
        //textureNum
        int textureNum = 0;
        steg::PLuaPushFromTable_J(6);
        steg::PLuaPop(&textureNum);

        //imgFile
        std::string* imgFile = new std::string("");
        InterfaceGame::usedStrings.push_back(imgFile);
        steg::PLuaPushFromTable_J(7);
        steg::PLuaPop_J(imgFile);

        comp = new GUI(name.c_str(), x, y, drawIndex, textureNum, imgFile->c_str(), (Canvas*)motherCanvas);
    }

    return comp;
}

GameComp* PLoadGameCompFromSourcesTable_J(int number, const std::string& sceneName)
{
    GameComp* comp = NULL;

    std::string className = "";
    steg::PLuaPushFromTable_J(number);              // +1
    //now, the arguments table is on stack top
    steg::PLuaPushFromTable_J("class");             // +1
    steg::PLuaPop_J(&className);                    // -1
    if(className == "Canvas")
    {
//        comp = (GameComp*)PLoadCanvas_J(sceneName);
        //当用到多重继承时，一定要用dynamic_cast进行转换，而不要用强制类型转换！！！切记！！！
        Canvas* canv = PLoadCanvas_J(sceneName);
        comp = dynamic_cast<GameComp*>(canv);
    }
    else if(className == "GUI")
    {
//        comp = (GameComp*)PLoadGUI_J(sceneName);
        comp = dynamic_cast<GameComp*>(PLoadGUI_J(sceneName));
    }

    steg::PLuaPop();                                // -1

    return comp;
}

DBG_Status PGetSDLRectFromTopTable_J(SDL_Rect* rect)
{
    DBG_Status status = DBG_OK;

    if(rect == NULL)
    {
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    int temp = 0;
    status |= steg::PLuaPushFromTable_J(1);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    rect->x = temp;

    status |= steg::PLuaPushFromTable_J(2);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    rect->y = temp;

    status |= steg::PLuaPushFromTable_J(3);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    rect->w = temp;

    status |= steg::PLuaPushFromTable_J(4);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    rect->h = temp;

    return status;
}

DBG_Status PGetSDLPointFromTopTable_J(SDL_Point* point)
{
    DBG_Status status = DBG_OK;

    if(point == NULL)
    {
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    int temp = 0;
    status |= steg::PLuaPushFromTable_J(1);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    point->x = temp;

    status |= steg::PLuaPushFromTable_J(2);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    point->y = temp;

    return status;
}

DBG_Status PGetSDLColorFromTopTable_J(SDL_Color* color)
{
    DBG_Status status = DBG_OK;

    if(color == NULL)
    {
        return DBG_ARG_ERR | DBG_NULL_PTR;
    }

    int temp = 0;
    status |= steg::PLuaPushFromTable_J(1);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    color->r = temp;

    status |= steg::PLuaPushFromTable_J(2);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    color->g = temp;

    status |= steg::PLuaPushFromTable_J(3);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    color->b = temp;

    status |= steg::PLuaPushFromTable_J(4);   // +1
    status |= steg::PLuaPop(&temp);           // -1
    color->a = temp;

    return status;
}

DBG_Status NewLuaProxy(std::string& name, std::string& filePath, void* ud)
{
    DBG_Status status = DBG_OK;
    if(ud == NULL)
        return DBG_ARG_ERR | DBG_NULL_PTR;

    LegalizeName(name);
    std::string className = GetClassName(name);
    if(filePath.empty())
    {
        filePath = "scripts/";
    }
    filePath += className;
    filePath += ".lua";

    //first, check naming collision
    steg::PLuaPushNil();                                            // +1
    status |= steg::PLuaPushFromTable_J(name.c_str());              // +1, actually, push from global
    while(!steg::PLuaTopIsNil())
    {
        NameAddOne(name);
        steg::PLuaPop();                            // -1
        steg::PLuaPushFromTable_J(name.c_str());    // +1
    }
    steg::PLuaPop(2);                                               // -2

    //and then, new one proxy
    status |= steg::PLuaDoScript_J(filePath.c_str());
    //now we have the className luaProxy class in lua global
    //create new table as proxy
    status |= steg::PLuaPushNewTable_J();                           // +1
    int proxyTablePos = steg::PLuaGetTop();

    //no need to use PCallProxyFunction here
    //because the code isn't very much
    steg::PLuaPushNil();                                            // +1
    status |= steg::PLuaPushFromTable_J(className.c_str());         // +1
    int classTablePos = steg::PLuaGetTop();
    //the class table is on stack top
//    PrintStackTop();
    steg::PCallLuaFucntionFromTable_J("New", "ttu", NULL, classTablePos, proxyTablePos, ud);

    //pop class table
    steg::PLuaPop();                                                // -1
    //pop nil
    steg::PLuaPop();                                                // -1

    //now, the proxy is on stack top
    //register it to global
    status |= steg::PLuaSetToGlobal_J(name.c_str());                // -1

    return status;
}

//temp: 合并之前的
//SplitString() function code from http://blog.csdn.net/glt3953/article/details/11115485
static std::vector<std::string> SplitString(std::string str, std::string pattern)
{
	std::vector<std::string> result;
	std::string::size_type pos;
	str += pattern;		//扩展字符串以方便操作
	int size = str.size();

	for(int i = 0; i < size; i++)
	{
		pos=str.find(pattern, i);
		if(pos < size)
		{
			std::string s = str.substr(i, pos-i);
			if(!s.empty())
                result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

DBG_Status PushFromFindPath(const std::string& findPath, const std::string& sceneName)
{
    DBG_Status status = DBG_OK;

    if(findPath.empty())
        return DBG_ARG_ERR;

    int top = steg::PLuaGetTop();

    std::vector<std::string> paths = SplitString(findPath, ".");
    if(findPath[0] == '.')  //find from motherScene luaProxy table
    {
        if(sceneName.empty())
            return DBG_ARG_ERR;

        steg::PLuaPushNil();                            // +1
        steg::PLuaPushFromTable_J(sceneName.c_str());   // +1
        for(int i = 0; i < paths.size(); i++)           // +size
        {
            steg::PLuaPushFromTable_J(paths[i].c_str());
        }
        lua_replace(steg::L, top + 1);                  // -1
        steg::PLuaPop(paths.size());                    // -size
    }
    else                    //find from global
    {
        steg::PLuaPushNil();                                // +1
        for(int i = 0; i < paths.size(); i++)
        {
            steg::PLuaPushFromTable_J(paths[i].c_str());    // +size
        }
        lua_replace(steg::L, top + 1);                      // -1
        steg::PLuaPop(paths.size() - 1);                    // -size+1
    }

    int a = steg::PLuaGetTop();

    return status;
}

}

