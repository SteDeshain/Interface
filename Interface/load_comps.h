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
GameComp* PLoadGameCompFromSourcesTable_J(int number);

DBG_Status PGetSDLRectFromTopTable_J(SDL_Rect* rect);
DBG_Status PGetSDLPointFromTopTable_J(SDL_Point* point);
DBG_Status PGetSDLColorFromTopTable_J(SDL_Color* color);

DBG_Status NewLuaProxy(std::string& name, std::string& filePath, void* ud);

}

#endif // __LOAD_COMPS_H__
