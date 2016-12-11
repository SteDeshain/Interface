#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <iostream>
#include <string>
#include <sstream>

namespace interface
{

bool IsNumber(std::string s);

std::string GetClassName(std::string name);
std::string& LegalizeName(std::string& name);
std::string& NameAddOne(std::string& name);   // +1s

void PrintStackTop();   //mostly it's a table
void PrintAllGlobal();

}

#endif // __TOOLS_H__
