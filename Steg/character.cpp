#include "character.h"

namespace steg
{

int Character::groundDetectorThinckness = 5;

SenseComp::SenseComp(SolidObjInfo info, SolidObj* fatherObj)
    :SolidObj(info), fatherObj(fatherObj)
{
    visible = true; //to show extra draw

    collideType = CollideC; //temp
    collidable = true;
    triggered = true;

    tag = tagSenseBox;
}

SolidObj* SenseComp::GetFatherObj()
{
    return fatherObj;
}

void SenseComp::ResetBox(stef::Vector3 position, stef::Vector3 halfSize)
{
    box.SetPosition(position);
    box.SetHalfSize(halfSize);
}

DBG_Status SenseComp::InitInScene(Scene* scene)
{
    DBG_Status status = DBG_OK;

    status |= GameComp::InitInScene(scene);
    if(status & DBG_REP_OPR)
        return status;

    visible = true;
    //temp
    box.SetHalfSize(stef::Vector3(ToMeter(20),
                                  ToMeter(20),
                                  ToMeter(20)));

    scene->solidObjs.push_back(this);
    //do not push box into the physics world
//    scene->pWorld.GetParticles().push_back(&box);

    return status;
}

DBG_Status SenseComp::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= SolidObj::Update(deltTick);

    //update position
    box.SetPosition(fatherObj->GetBox().GetPosition() + centerOffset);
    //set draw rect to enable collision detect
    SetDrawSize(ToPix(box.GetHalfSize().x * 2), ToPix((box.GetHalfSize().y + box.GetHalfSize().z) * 2));

    return status;
}

void SenseComp::DrawGroundTouch(Camera* camera)
{
    return;
}

Character::Character(SolidObjInfo info, const char* imgFile, int textureNum)
    :DrawableComp(textureNum, imgFile, NULL),
     SolidObj(info, imgFile, textureNum)
{
    groundDetector = new SenseComp(info, this);
}

SenseComp* Character::GetGroundDetector()
{
    return groundDetector;
}

bool Character::IsGrounded()
{
    return grounded;
}

DBG_Status Character::InitInScene(Scene* scene)
{
    DBG_Status status = DBG_OK;

    status |= SolidObj::InitInScene(scene);
    status |= groundDetector->InitInScene(scene);

    groundDetector->centerOffset = stef::Vector3(0, 0,
                                                -1 * (box.GetHalfSize().z + ToMeter(groundDetectorThinckness) / 2));
    groundDetector->box.SetHalfSize(stef::Vector3(box.GetHalfSize().x * 0.9,
                                                  box.GetHalfSize().y * 0.9,
                                                  ToMeter(groundDetectorThinckness) / 2));

    return status;
}

//temp
DBG_Status Character::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= SolidObj::Update(deltTick);

//    groundDetector->box.SetPosition(box.GetPosition() + groundDetector->centerOffset);

    //temp
    grounded = groundDetector->box.GetContactObjs().size() != 0;

    return status;
}

}
