#include "gui_scene.h"
#include "tools.h"
#include "stdlib.h"
#include "interface_gui.h"
#include "load_comps.h"
//tmp
//#include "event_handler.h"

namespace interface
{

GuiScene::GuiScene(const char* name, SDL_Renderer* render)
    :InterfaceScene(name, 0, 0, render), steg::Scene(0, 0, render)
{
}

GuiScene::~GuiScene()
{
}

DBG_Status GuiScene::InitScene()
{
    DBG_Status status = DBG_OK;

    //new luaProxy
    std::string filePath = "scripts/scenes/";
    status |= NewLuaProxy(name, filePath, this);

    //now, load the GameComps registered in script's sources table
    steg::PLuaPushNil();                                            // +1
    steg::PLuaPushFromTable_J(name.c_str());                        // +1
    //now, the proxy table is on stack top
    steg::PLuaPushFromTable_J("sources");                           // +1
    //now, the proxy's sources table is on stack top
    //get the GameComp number
    int compNumber = 0;
    steg::PLuaPushFromTable_J("length");    // +1
    steg::PLuaPop(&compNumber);             // -1

    steg::GameComp* comp = NULL;
    for(int i = 0; i < compNumber; i++)
    {
        comp = PLoadGameCompFromSourcesTable_J(i + 1, name);
        if(comp)
        {
            (*this) << comp;
            //register to sources table
//            std::string name = comp->GetNameString();
//            std::string name = comp->name;
            if(!(comp->GetName()).empty())    //empty name?
            {
                std::string tableKeyName = "";
                steg::PLuaPushFromTable_J(i + 1);           // +1
                steg::PLuaPushFromTable_J("name");          // +1
                steg::PLuaPop_J(&tableKeyName);             // -1
                steg::PLuaPop();                            // -1

                steg::PLuaPushNil();                            // +1
                //get object luaProxy table
//                steg::PLuaPushFromTable_J(comp->GetName());     // +1
                steg::PLuaPushFromTable_J(comp->GetNameCStr());
                //remove nil
                lua_remove(steg::L, -2);                        // -1
                steg::PLuaSetToTable_J(tableKeyName.c_str());   // -1

                //tmp:
//                luaL_dostring(steg::L,
//                              "print(welcomeScene_1.sources)");
//                luaL_dostring(steg::L,
//                              "print(unpack(welcomeScene_1.sources))");
//                luaL_dostring(steg::L,
//                              "print(welcomeScene_1.sources.logoCanv_1)");
//                luaL_dostring(steg::L,
//                              "print(welcomeScene_1.sources.logoCanv_1.ud)");

            }
        }
    }

    //pop sources table
    steg::PLuaPop();                                                // -1
    //pop proxy table
    steg::PLuaPop();                                                // -1
    //pop nil
    steg::PLuaPop();                                                // -1
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

//DBG_Status GuiScene::HandleInput()
//{
//    using namespace steg;
//
//    DBG_Status status = DBG_OK;
//
//    status |= InterfaceScene::HandleInput();
//
//    //...
//    //tmp
//    void* canvas = NULL;
//    PLuaPushNil();                          // +1
//    PLuaPushFromTable_J("welcomeScene_1");
//    PLuaPushFromTable_J("sources");
//    PLuaPushFromTable_J("logoCanv_1");
//    PLuaPushFromTable_J("ud");
//    PLuaPop(&canvas);
//    PLuaPop(4);
//    if(inputHandler->KeyPressed(SDL_SCANCODE_Z))
//        PushHideCanvasEvent(canvas);
//    if(inputHandler->KeyPressed(SDL_SCANCODE_X))
//        PushShowCanvasEvent(canvas);
//
//    return status;
//}

}
