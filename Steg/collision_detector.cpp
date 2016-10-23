#include "collision_detector.h"

namespace steg
{

CollideInfo CollisionDetect(const stef::Box& a, const stef::Box& b)
{
    CollideInfo info;
    info.collideNormal = stef::Vector3::ZERO;
    info.penetration = 0;

    using stef::real;
    real aLeft = a.GetLeft();
    real aRight = a.GetRight();
    real aTop = a.GetTop();
    real aBottom = a.GetBottom();
    real aFront = a.GetFront();
    real aBack = a.GetBack();

    real aLastLeft = a.GetLastLeft();
    real aLastRight = a.GetLastRight();
    real aLastTop = a.GetLastTop();
    real aLastBottom = a.GetLastBottom();
    real aLastFront = a.GetLastFront();
    real aLastBack = a.GetLastBack();

    real bLeft = b.GetLeft();
    real bRight = b.GetRight();
    real bTop = b.GetTop();
    real bBottom = b.GetBottom();
    real bFront = b.GetFront();
    real bBack = b.GetBack();

    real bLastLeft = b.GetLastLeft();
    real bLastRight = b.GetLastRight();
    real bLastTop = b.GetLastTop();
    real bLastBottom = b.GetLastBottom();
    real bLastFront = b.GetLastFront();
    real bLastBack = b.GetLastBack();

    real xTime = 0;
    real yTime = 0;
    real zTime = 0;

    if(aRight <= bLeft || aLeft >= bRight ||
       aTop <= bBottom || aBottom >= bTop ||
       aFront <= bBack || aBack >= bFront)
    {
        //no collision
    }
    else
    {
        //there is collision
        //...
        //b as the zero point
        stef::Vector3 lastConnect = a.GetLastPosition() - b.GetLastPosition();
        stef::Vector3 connect = a.GetPosition() - b.GetPosition();
        stef::Vector3 collideDirection = lastConnect - connect;
        if(collideDirection.x == 0 && collideDirection.y == 0 && collideDirection.z == 0)
        {
            //no collision
            //micro collision may cause this situation
            return info;
        }
        //I卦限
        if(collideDirection.x >= 0 && collideDirection.y >= 0 && collideDirection.z >= 0)
        {
            if(collideDirection.x)
                xTime = real_abs((aLastLeft - bLastRight) / collideDirection.x);
            else
                xTime = REAL_MAX;

            if(collideDirection.y)
                yTime = real_abs((aLastBack - bLastFront) / collideDirection.y);
            else
                yTime = REAL_MAX;

            if(collideDirection.z)
                zTime = real_abs((aLastBottom - bLastTop) / collideDirection.z);
            else
                zTime = REAL_MAX;

            if((xTime <= yTime && xTime <= zTime))
            {
                info.collideNormal = stef::Vector3::RIGHT;
                info.penetration = real_abs(bRight - aLeft);
            }
            else if(yTime <= xTime && yTime <= zTime)
            {
                info.collideNormal = stef::Vector3::FRONT;
                info.penetration = real_abs(bFront - aBack);
            }
            else if(zTime <= xTime && zTime <= yTime)
//            else
            {
                info.collideNormal = stef::Vector3::TOP;
                info.penetration = real_abs(bTop - aBottom);
            }
        }
        //II卦限
        else if(collideDirection.x < 0 && collideDirection.y >= 0 && collideDirection.z >= 0)
        {
            if(collideDirection.x)
                xTime = real_abs((aLastRight - bLastLeft) / collideDirection.x);
            else
                xTime = REAL_MAX;

            if(collideDirection.y)
                yTime = real_abs((aLastBack - bLastFront) / collideDirection.y);
            else
                yTime = REAL_MAX;

            if(collideDirection.z)
                zTime = real_abs((aLastBottom - bLastTop) / collideDirection.z);
            else
                zTime = REAL_MAX;

            if((xTime <= yTime && xTime <= zTime))
            {
                info.collideNormal = stef::Vector3::LEFT;
                info.penetration = real_abs(aRight - bLeft);
            }
            else if(yTime <= xTime && yTime <= zTime)
            {
                info.collideNormal = stef::Vector3::FRONT;
                info.penetration = real_abs(bFront - aBack);
            }
            else if(zTime <= xTime && zTime <= yTime)
//            else
            {
                info.collideNormal = stef::Vector3::TOP;
                info.penetration = real_abs(bTop - aBottom);
            }
        }
        //III卦限
        else if(collideDirection.x < 0 && collideDirection.y < 0 && collideDirection.z >= 0)
        {
            if(collideDirection.x)
                xTime = real_abs((aLastRight - bLastLeft) / collideDirection.x);
            else
                xTime = REAL_MAX;

            if(collideDirection.y)
                yTime = real_abs((aLastFront - bLastBack) / collideDirection.y);
            else
                yTime = REAL_MAX;

            if(collideDirection.z)
                zTime = real_abs((aLastBottom - bLastTop) / collideDirection.z);
            else
                zTime = REAL_MAX;

            if((xTime <= yTime && xTime <= zTime))
            {
                info.collideNormal = stef::Vector3::LEFT;
                info.penetration = real_abs(aRight - bLeft);
            }
            else if(yTime <= xTime && yTime <= zTime)
            {
                info.collideNormal = stef::Vector3::BACK;
                info.penetration = real_abs(aFront - bBack);
            }
            else if(zTime <= xTime && zTime <= yTime)
//            else
            {
                info.collideNormal = stef::Vector3::TOP;
                info.penetration = real_abs(bTop - aBottom);
            }
        }
        //IV卦限
        else if(collideDirection.x >= 0 && collideDirection.y < 0 && collideDirection.z >= 0)
        {
            if(collideDirection.x)
                xTime = real_abs((aLastLeft - bLastRight) / collideDirection.x);
            else
                xTime = REAL_MAX;

            if(collideDirection.y)
                yTime = real_abs((aLastFront - bLastBack) / collideDirection.y);
            else
                yTime = REAL_MAX;

            if(collideDirection.z)
                zTime = real_abs((aLastBottom - bLastTop) / collideDirection.z);
            else
                zTime = REAL_MAX;

            if((xTime <= yTime && xTime <= zTime))
            {
                info.collideNormal = stef::Vector3::RIGHT;
                info.penetration = real_abs(aLeft - bRight);
            }
            else if(yTime <= xTime && yTime <= zTime)
            {
                info.collideNormal = stef::Vector3::BACK;
                info.penetration = real_abs(aFront - bBack);
            }
            else if(zTime <= xTime && zTime <= yTime)
//            else
            {
                info.collideNormal = stef::Vector3::TOP;
                info.penetration = real_abs(bTop - aBottom);
            }
        }
        //V卦限
        else if(collideDirection.x >= 0 && collideDirection.y >= 0 && collideDirection.z < 0)
        {
            if(collideDirection.x)
                xTime = real_abs((aLastLeft - bLastRight) / collideDirection.x);
            else
                xTime = REAL_MAX;

            if(collideDirection.y)
                yTime = real_abs((aLastBack - bLastFront) / collideDirection.y);
            else
                yTime = REAL_MAX;

            if(collideDirection.z)
                zTime = real_abs((aLastTop - bLastBottom) / collideDirection.z);
            else
                zTime = REAL_MAX;

            if((xTime <= yTime && xTime <= zTime))
            {
                info.collideNormal = stef::Vector3::RIGHT;
                info.penetration = real_abs(aLeft - bRight);
            }
            else if(yTime <= xTime && yTime <= zTime)
            {
                info.collideNormal = stef::Vector3::FRONT;
                info.penetration = real_abs(aBack - bFront);
            }
            else if(zTime <= xTime && zTime <= yTime)
//            else
            {
                info.collideNormal = stef::Vector3::BOTTOM;
                info.penetration = real_abs(aTop - bBottom);
            }
        }
        //VI卦限
        else if(collideDirection.x < 0 && collideDirection.y >= 0 && collideDirection.z < 0)
        {
            if(collideDirection.x)
                xTime = real_abs((aLastRight - bLastLeft) / collideDirection.x);
            else
                xTime = REAL_MAX;

            if(collideDirection.y)
                yTime = real_abs((aLastBack - bLastFront) / collideDirection.y);
            else
                yTime = REAL_MAX;

            if(collideDirection.z)
                zTime = real_abs((aLastTop - bLastBottom) / collideDirection.z);
            else
                zTime = REAL_MAX;

            if((xTime <= yTime && xTime <= zTime))
            {
                info.collideNormal = stef::Vector3::LEFT;
                info.penetration = real_abs(aRight - bLeft);
            }
            else if(yTime <= xTime && yTime <= zTime)
            {
                info.collideNormal = stef::Vector3::FRONT;
                info.penetration = real_abs(aBack - bFront);
            }
            else if(zTime <= xTime && zTime <= yTime)
//            else
            {
                info.collideNormal = stef::Vector3::BOTTOM;
                info.penetration = real_abs(aTop - bBottom);
            }
        }
        //VII卦限
        else if(collideDirection.x < 0 && collideDirection.y < 0 && collideDirection.z < 0)
        {
            if(collideDirection.x)
                xTime = real_abs((aLastRight - bLastLeft) / collideDirection.x);
            else
                xTime = REAL_MAX;

            if(collideDirection.y)
                yTime = real_abs((aLastFront - bLastBack) / collideDirection.y);
            else
                yTime = REAL_MAX;

            if(collideDirection.z)
                zTime = real_abs((aLastTop - bLastBottom) / collideDirection.z);
            else
                zTime = REAL_MAX;

            if((xTime <= yTime && xTime <= zTime))
            {
                info.collideNormal = stef::Vector3::LEFT;
                info.penetration = real_abs(aRight - bLeft);
            }
            else if(yTime <= xTime && yTime <= zTime)
            {
                info.collideNormal = stef::Vector3::BACK;
                info.penetration = real_abs(aFront - bBack);
            }
            else if(zTime <= xTime && zTime <= yTime)
//            else
            {
                info.collideNormal = stef::Vector3::BOTTOM;
                info.penetration = real_abs(aTop - bBottom);
            }
        }
        //VIII卦限
        else if(collideDirection.x >= 0 && collideDirection.y < 0 && collideDirection.z < 0)
        {
            if(collideDirection.x)
                xTime = real_abs((aLastLeft - bLastRight) / collideDirection.x);
            else
                xTime = REAL_MAX;

            if(collideDirection.y)
                yTime = real_abs((aLastFront - bLastBack) / collideDirection.y);
            else
                yTime = REAL_MAX;

            if(collideDirection.z)
                zTime = real_abs((aLastTop - bLastBottom) / collideDirection.z);
            else
                zTime = REAL_MAX;

            if((xTime <= yTime && xTime <= zTime))
            {
                info.collideNormal = stef::Vector3::RIGHT;
                info.penetration = real_abs(aLeft - bRight);
            }
            else if(yTime <= xTime && yTime <= zTime)
            {
                info.collideNormal = stef::Vector3::BACK;
                info.penetration = real_abs(aFront - bBack);
            }
            else if(zTime <= xTime && zTime <= yTime)
//            else
            {
                info.collideNormal = stef::Vector3::BOTTOM;
                info.penetration = real_abs(aTop - bBottom);
            }
        }
    }

    return info;
}

}
