#include "multi_draw_solid_obj.h"

namespace steg
{

MultiDrawSolidObj::MultiDrawSolidObj(const char* name, SolidObjInfo info,
                                     int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects,
                                     const char* imgFile, int textureNum)
    :MultiDrawSolidObj(info, destNum, destRects, sourceRects, imgFile, textureNum)
{
    this->name = name;
}

MultiDrawSolidObj::MultiDrawSolidObj(SolidObjInfo info, int destNum, SDL_Rect* destRects, SDL_Rect* sourceRects,
                                     const char* imgFile, int textureNum)
    :DrawableComp(textureNum, imgFile, NULL),
     MultiDrawComp(0, 0, destNum, destRects, sourceRects, textureNum, imgFile, NULL),
     SolidObj(info, imgFile, textureNum)
{
}

DBG_Status MultiDrawSolidObj::InitInScene(Scene* scene)
{
    DBG_Status status = DBG_OK;

    status |= MultiDrawComp::InitInScene(scene);
    if(status & DBG_REP_OPR)
        return status;

//    status |= SolidObj::InitInScene(scene);
    if(box.GetHalfSize().x == 0 || box.GetHalfSize().z == 0)    //auto volume
    {
        if(drawRect.w == 0)
        {
            //wrong arguement
            ENG_LogError("Bad arguement: You can't create a SolidObj with both NULL imgFile and auto volume!");
            //init the volume with width as a cube
            stef::real halfWidth = box.GetHalfSize().y;
            box.SetHalfSize(stef::Vector3(halfWidth, halfWidth, halfWidth));
        }
        else
        {
            box.SetHalfSize(stef::Vector3(ToMeter(drawRect.w) / 2,
                                          box.GetHalfSize().y,
                                          ToMeter(drawRect.h) / 2 - box.GetHalfSize().y));
        }
        box.SetPosition(box.GetPosition().x, box.GetPosition().y, box.GetPosition().z + box.GetHalfSize().z);
    }

    scene->solidObjs.push_back(this);
    scene->pWorld.GetParticles().push_back(&box);

    return status;
}

DBG_Status MultiDrawSolidObj::DumpOutOfScene()
{
    DBG_Status status = DBG_OK;

    status |= SolidObj::DumpOutOfScene();
    return status;
}

DBG_Status MultiDrawSolidObj::Draw(Uint32 deltTick, Camera* camera)
{
    DBG_Status status = DBG_OK;

    SetAbsPos(ToPix(box.GetPosition().x),
              ToPix(box.GetPosition().y - box.GetPosition().z));

    status |= MultiDrawComp::Draw(deltTick, camera);

    return status;
}

}
