#include "font.h"
#include "log.h"

namespace steg
{

DBG_Status FontInit()
{
    DBG_Status status = DBG_OK;

    ENG_LogInfo("Font module initializing...");

	if(!TTF_WasInit() && TTF_Init() == -1)
	{
        ENG_LogErrors("Cannot init Font system: ", TTF_GetError());
	}
	else
    {
        ENG_LogInfo("Font module initialize successfully!");
    }

    return status;
}

void FontQuit()
{
	ENG_LogInfo("Font module shutting down...");
    if(TTF_WasInit())
    {
        TTF_Quit();
        ENG_LogInfo("Font module shut down successfully!");
    }
    else
    {
        ENG_LogInfo("Font module already shut down!");
    }
}

Font::Font(const char* fontFile, unsigned int minSize, unsigned int maxSize)
{
    this->minSize = minSize;
    if(maxSize < minSize)
    {
        sizeCount = 1;
    }
    else
    {
        sizeCount = maxSize - minSize + 1;
    }

    fonts = new TTF_Font*[sizeCount];
    if(fonts == NULL)
    {
        ENG_LogError("Cannot malloc TTF_Font* for Font class.");
    }
    else
    {
        for(int i = 0; i < sizeCount; i++)
        {
            fonts[i] = NULL;
            fonts[i] = TTF_OpenFont(fontFile, minSize + i);
            if(fonts[i] == NULL)
            {
                ENG_LogErrors("Cannot open font file: ", TTF_GetError());
            }
        }
    }
}

Font::~Font()
{
    if(fonts)
    {
        for(int i = 0; i < sizeCount; i++)
        {
            if(fonts[i])
            {
                TTF_CloseFont(fonts[i]);
            }
        }
        delete[] fonts;
        fonts = NULL;
    }
}

TTF_Font* Font::operator[](int i)
{
    if(i < minSize)
    {
        return fonts[0];
    }
    else if(i > minSize + sizeCount - 1)
    {
        return fonts[sizeCount - 1];
    }
    else
    {
        return fonts[i - minSize];
    }
}

}
