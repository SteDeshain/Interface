//collide_type.h
//created by SteDeshain
//created on 2016/08/14
//latest updated on 2016/08/14

#ifndef __COLLIDE_TYPE_H__
#define __COLLIDE_TYPE_H__

#include "SDL2/SDL.h"
#include "status.h"

#if 1

#define CollideType     Uint8

#define CollideNon      0b00000000
#define CollideA        0b00000001
#define CollideB        0b00000010
#define CollideC        0b00000100
#define CollideD        0b00001000
#define CollideE        0b00010000
#define CollideF        0b00100000
#define CollideG        0b01000000
#define CollideH        0b10000000
#define CollideAll      0b11111111
    #if 0
    #define CollideType     Uint16

    #define CollideNon      0b0000000000000000
    #define CollideA        0b0000000000000001
    #define CollideB        0b0000000000000010
    #define CollideC        0b0000000000000100
    #define CollideD        0b0000000000001000
    #define CollideE        0b0000000000010000
    #define CollideF        0b0000000000100000
    #define CollideG        0b0000000001000000
    #define CollideH        0b0000000010000000
    #define CollideI        0b0000000100000000
    #define CollideJ        0b0000001000000000
    #define CollideK        0b0000010000000000
    #define CollideL        0b0000100000000000
    #define CollideM        0b0001000000000000
    #define CollideN        0b0010000000000000
    #define CollideO        0b0100000000000000
    #define CollideP        0b1000000000000000
    #define CollideAll      0b1111111111111111
    #endif // 0
#else
namespace steg
{

enum CollideType: Uint16
{
    CollideNon =    0b0000000000000000,
    CollideA =      0b0000000000000001,
    CollideB =      0b0000000000000010,
    CollideC =      0b0000000000000100,
    CollideD =      0b0000000000001000,
    CollideE =      0b0000000000010000,
    CollideF =      0b0000000000100000,
    CollideG =      0b0000000001000000,
    CollideH =      0b0000000010000000,
    CollideI =      0b0000000100000000,
    CollideJ =      0b0000001000000000,
    CollideK =      0b0000010000000000,
    CollideL =      0b0000100000000000,
    CollideM =      0b0001000000000000,
    CollideN =      0b0010000000000000,
    CollideO =      0b0100000000000000,
    CollideP =      0b1000000000000000,
    CollideAll =    0b1111111111111111,
};

}
#endif // 1

namespace steg
{

static unsigned int CollidePairs[8][8];
/*
{    a  b  c  d  e  f  g  h
   a{1, 0, 0, 0, 0, 0, 0, 0},
   b{0, 1, 0, 0, 0, 0, 0, 0},
   c{0, 0, 1, 0, 0, 0, 0, 0},
   d{0, 0, 0, 1, 0, 0, 0, 0},
   e{0, 0, 0, 0, 1, 0, 0, 0},
   f{0, 0, 0, 0, 0, 1, 0, 0},
   g{0, 0, 0, 0, 0, 0, 1, 0},
   h{0, 0, 0, 0, 0, 0, 0, 1}
};
*/

DBG_Status CollideInit();
void CollideQuit();

bool IsCollided(CollideType x, CollideType y);

CollideType GetCollideTypeFromString(const char* typeString);

DBG_Status SetCollidePair(char x, char y);

DBG_Status RemoveCollidePair(char x, char y);

void ClearAllCollidePairs();
void EnableAllCollidePairs();

}

#endif // __COLLIDE_TYPE_H__
