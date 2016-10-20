#include "color.h"

namespace steg
{

unsigned int ToRGBA(RGBColor c)
{
    return ((c << 8) + 0x000000FF);
}

SDL_Color GetColor(RGBColor c)
{
    return SDL_Color{c >> 16, (c >> 8) & 0xFF, c & 0xFF, 0xFF};
}

}
