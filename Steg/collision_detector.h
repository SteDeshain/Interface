#ifndef __COLLISION_DETECTOR_H__
#define __COLLISION_DETECTOR_H__

#include "core.h"
#include "box.h"

namespace steg
{

struct CollideInfo
{
    stef::Vector3 collideNormal;    //relative to first Box
    stef::real penetration;
};

CollideInfo CollisionDetect(const stef::Box& a, const stef::Box& b);

}

#endif // __COLLISION_DETECTOR_H__
