#ifndef __LOAD_COMPS_H__
#define __LOAD_COMPS_H__

#include "log.h"
#include "status.h"
#include "interface_comp.h"
#include "interface_gui.h"
#include "string"

namespace interface
{

//assuming the sources table is on stack top
//number is the index of sources table
steg::GameComp* PLoadGameCompFromSourcesTable_J(int number, const std::string& sceneName);

DBG_Status PGetSDLRectFromTopTable_J(SDL_Rect* rect);
DBG_Status PGetSDLPointFromTopTable_J(SDL_Point* point);
DBG_Status PGetSDLColorFromTopTable_J(SDL_Color* color);

DBG_Status NewLuaProxy(std::string& name, std::string& filePath, void* ud);

DBG_Status PushFromFindPath(const std::string& findPath, const std::string& sceneName); // +1

}

#endif // __LOAD_COMPS_H__
