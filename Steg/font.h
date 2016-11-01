#ifndef __STEG_FONT_H__
#define __STEG_FONT_H__

#include "SDL2/SDL_ttf.h"
#include "status.h"

namespace steg
{

DBG_Status FontInit();
void FontQuit();

class Font
{

public:
//    Font(const char* fontFile);
    Font(const char* fontFile, unsigned int minSize = 12, unsigned int maxSize = 50);

    ~Font();

    TTF_Font* operator[](int i);

protected:
    TTF_Font** fonts = NULL;

private:
    unsigned int sizeCount;
    unsigned int minSize;

};

}

#endif // __STEG_FONT_H__
