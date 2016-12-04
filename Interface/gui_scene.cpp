#include "gui_scene.h"

namespace interface
{

GuiScene::GuiScene(const char* name, SDL_Renderer* render)
    :InterfaceScene(name, 0, 0, render)
{
}

GuiScene::~GuiScene()
{
}

DBG_Status GuiScene::InitScene()
{
    DBG_Status status = DBG_OK;

    //register lua functions according to name
    //...
    std::string filePath = "scripts/scenes/";
    filePath += name;
    filePath += ".lua";

    std::string funPath = name + "|init";
    std::string newFunName = name + "Init";
    steg::PRegisterLuaFunction_J(filePath.c_str(), funPath.c_str(), newFunName.c_str());

    funPath = name + "|update";
    newFunName = name + "Update";
    steg::PRegisterLuaFunction_J(filePath.c_str(), funPath.c_str(), newFunName.c_str());

//temp
//**********************************************************************//
//**********************************************************************//
//    REMEMBER TO CHECK STACK HERE! BECAUSE HERE USES LOTS OF STACK
//**********************************************************************//
//**********************************************************************//
    //init scene
    //...
    newFunName = name + "Init";
    steg::PCallLuaFunctionWithUid_J(newFunName.c_str(), "", NULL);
    //make sure do that script
    steg::PLuaDoScript(filePath.c_str());
    //and then, get the sourecs table and construct them
    lua_pushnil(steg::L);                                                       // +1
    steg::PLuaPushFromTable_J(name.c_str());                                    // +1
    //now, the table is on the stack top
    //step into loop

    lua_pop(steg::L, 2);                                                        // -2
    return status;
}

DBG_Status GuiScene::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= InterfaceScene::Update(deltTick);

    //call lua script update
    steg::PCallLuaFunctionWithUid_J((name + "Update").c_str(), "un", NULL, this, deltTick);

    return status;
}

DBG_Status GuiScene::UpdatePhysics(Uint32 reminingTick)
{
    DBG_Status status = DBG_OK;

    //do nothing!

    return status;
}

}
