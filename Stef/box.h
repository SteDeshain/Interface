#ifndef __STEF_BOX_H__
#define __STEF_BOX_H__

#include "core.h"
#include "particle.h"

namespace stef
{

class Box: public Particle
{

protected:
    Vector3 halfSize;
    Vector3 lastHalfSize = Vector3::ZERO;

public:
    void SetHalfSize(Vector3 halfSize)
    {
        this->halfSize = halfSize;
        if(lastHalfSize == Vector3::ZERO)
            lastHalfSize = halfSize;
    }

    Vector3 GetHalfSize() const
    {
        return halfSize;
    }

    real GetTop() const
    {
        return position.z + halfSize.z;
    }

    real GetBottom() const
    {
        return position.z - halfSize.z;
    }

    real GetFront() const
    {
        return position.y + halfSize.y;
    }

    real GetBack() const
    {
        return position.y - halfSize.y;
    }

    real GetLeft() const
    {
        return position.x - halfSize.x;
    }

    real GetRight() const
    {
        return position.x + halfSize.x;
    }

    //last
    real GetLastTop() const
    {
        return lastPosition.z + lastHalfSize.z;
    }

    real GetLastBottom() const
    {
        return lastPosition.z - lastHalfSize.z;
    }

    real GetLastFront() const
    {
        return lastPosition.y + lastHalfSize.y;
    }

    real GetLastBack() const
    {
        return lastPosition.y - lastHalfSize.y;
    }

    real GetLastLeft() const
    {
        return lastPosition.x - lastHalfSize.x;
    }

    real GetLastRight() const
    {
        return lastPosition.x + lastHalfSize.x;
    }

    virtual void RestoreLastFrameValue()
    {
        Particle::RestoreLastFrameValue();
        lastHalfSize = halfSize;
    }

};

}

#endif  // __STEF_BOX_H__
