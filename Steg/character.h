#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "solid_obj.h"

namespace steg
{

class SenseComp: public SolidObj
{
    friend class Character;

public:
    SenseComp(SolidObjInfo info, SolidObj* fatherObj);

    SolidObj* GetFatherObj();
//    void SetFatherObj(SolidObj* obj);

    void ResetBox(stef::Vector3 position, stef::Vector3 halfSize);

protected:
    SolidObj* fatherObj;
    stef::Vector3 centerOffset = stef::Vector3::ZERO; //offset from fatherObj's center

    virtual DBG_Status InitInScene(Scene* scene);
    virtual DBG_Status Update(Uint32 deltTick);

    virtual void DrawGroundTouch(Camera* camera);

};

class Character: public SolidObj
{

public:
    static int groundDetectorThinckness;

public:
    Character(SolidObjInfo info, const char* imgFile, int textureNum);

    SenseComp* GetGroundDetector();

    bool IsGrounded();

protected:
    SenseComp* groundDetector;
    bool grounded;

    virtual DBG_Status InitInScene(Scene* scene);
    virtual DBG_Status Update(Uint32 deltTick);

};

}

#endif // __CHARACTER_H__