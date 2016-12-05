#include "tools.h"

namespace interface
{

bool IsNumber(std::string s)
{
#if 0
//codes from: http://blog.csdn.net/makenothing/article/details/40018731
    std::stringstream sin(s);
//    double d;
    int d;
    char c;
    if(!(sin >> d))
        return false;
    if (sin >> c)
        return false;
    return true;
#else
    for(int i = 0; i < s.length(); i++)
    {
        if(s[i] < '0' || s[i] > '9')
            return false;
    }
    return true;
#endif
}

std::string GetClassName(std::string name)
{
    std::string className = name;
    size_t pos = name.find_first_of('_');
    if(pos == std::string::npos)    //name is only the class name
    {
        //no change to className
    }
    else                            //name may have suffix
    {
        className.erase(pos);
    }
    return className;
}

std::string& LegalizeName(std::string& name)
{
    size_t pos = name.find_first_of('_');
    if(pos == std::string::npos)    //name is only the class name
    {
        //change name
        name += "_1";   // +1s
    }
    else                            //name may have suffix
    {
        std::string suffix = name.substr(pos + 1);
        if(suffix.empty())          //name still have no suffix, like "welcomeScene_"
        {
            name += '1';    // +1s
        }
        else
        {
            if(IsNumber(suffix))    //number suffix
            {
                //erase leading zero
                int zeroNumber = 0;
                for(int i = 0; i < suffix.length() - 1; i++)    //最后留一个
                {
                    if(suffix[i] == '0')
                        zeroNumber++;
                    else
                        break;
                }
                name.erase(pos + 1, zeroNumber);
            }
        }
    }

    return name;
}

std::string& NameAddOne(std::string& name)
{
    size_t pos = name.find_first_of('_');
    if(pos == std::string::npos)
    {
        return name;    //no suffix
    }
    else                            //name may have suffix
    {
        std::string suffix = name.substr(pos + 1);
        if(suffix.empty())          //name still have no suffix, like "welcomeScene_"
        {
            return name;    //no suffix
        }
        else
        {
            if(IsNumber(suffix))    //number suffix
            {
                int suffixNumber = 0;
                std::stringstream sin;
                sin << suffix;
                sin >> suffixNumber;
                suffixNumber++;

                sin.clear();
                sin << suffixNumber;
                sin >> suffix;  //new suffix

                name.replace(pos + 1, std::string::npos, suffix);
            }
            else    //string suffix
            {
                size_t lastPos = name.find_last_of('_');
                if(lastPos == pos)
                {
                    //suffix has no number suffix
                    //"welcomeScene_start"
                    name += "_1";
                }
                else
                {
                    if(lastPos == name.length() - 1)
                    {
                        //"welcomeScene_start_"
                        name += '1';
                    }
                    else
                    {
                        std::string lastSuffix = name.substr(lastPos + 1);
                        //"welcomeScene_start_12"
                        if(IsNumber(lastSuffix))
                        {
                            int lastNumber = 0;
                            std::stringstream sin;
                            sin << lastSuffix;
                            sin >> lastNumber;
                            lastNumber++;

                            sin.clear();
                            sin << lastNumber;
                            sin >> lastSuffix;  //new suffix of suffix

                            name.replace(lastPos + 1, std::string::npos, lastSuffix);
                        }
                        //"welcomeScene_start_suf"
                        else
                        {
                            name += "_1";
                        }
                    }
                }
            }
        }
    }
    return name;
}

}
