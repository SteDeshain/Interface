#include "collide_type.h"
#include "string.h"
#include "log.h"

namespace steg
{

DBG_Status CollideInit()
{
    ENG_LogInfo("Collision module initializing...");

    ClearAllCollidePairs();

    ENG_LogInfo("Collision module initialize successfully!");

    return DBG_OK;
}

void CollideQuit()
{
	ENG_LogInfo("Collision module shutting down...");
	ENG_LogInfo("Collision module shut down successfully!");
}

bool IsCollided(CollideType x, CollideType y)
{
    int xCount = 0;
    char xArray[8];
    for(int i = 0; i < 8; i++)
    {
        if(x & (1 << i))
        {
            xArray[xCount] = i;
            xCount++;
        }
    }

    int yCount = 0;
    char yArray[8];
    for(int i = 0; i < 8; i++)
    {
        if(y & (1 << i))
        {
            yArray[yCount] = i;
            yCount++;
        }
    }

    for(int i = 0; i < xCount; i++)
    {
        for(int j = 0; j < yCount; j++)
        {
            if(CollidePairs[xArray[i]][yArray[j]] || CollidePairs[yArray[j]][xArray[i]])
                return true;
        }
    }
    return false;
}

DBG_Status SetCollidePair(char x, char y)
{
    DBG_Status status = DBG_OK;

    if(x < 'a' || x > 'h' || y < 'a' || y > 'h')
    {
        status |= DBG_ARG_ERR;
        ENG_LogError("Wrong argument for CollidePair, must be within from 'a' to 'h'.");
        return status;
    }

    x -= 'a';
    y -= 'a';

    CollidePairs[x][y] = 1;
    CollidePairs[y][x] = 1;

    return status;
}

DBG_Status RemoveCollidePair(char x, char y)
{
    DBG_Status status = DBG_OK;

    if(x < 'a' || x > 'h' || y < 'a' || y > 'h')
    {
        status |= DBG_ARG_ERR;
        ENG_LogError("Wrong argument for CollidePair, must be within from 'a' to 'h'.");
        return status;
    }

    x -= 'a';
    y -= 'a';

    CollidePairs[x][y] = 0;
    CollidePairs[y][x] = 0;

    return status;
}

void ClearAllCollidePairs()
{
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            CollidePairs[j][i] = 0;
        }
    }
}

void EnableAllCollidePairs()
{
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            CollidePairs[j][i] = 1;
        }
    }
}

CollideType GetCollideTypeFromString(const char* typeString)
{
    CollideType type = CollideNon;

    if(NULL == typeString)
        return type;
    else
    {
        int stringLength = strlen(typeString);
        for(int i = 0; i < stringLength; i++)
        {
            char c = typeString[i] - 'a';
//            if(c < 0 || c > 15)
            if(c < 0 || c > 7)
                continue;
            else
                type |= 0b00000001 << c;
        }
    }

    return type;
}

}
