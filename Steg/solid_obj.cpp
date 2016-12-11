#include "solid_obj.h"
#include "color.h"

namespace steg
{

void SolidObjInfo::Clear()
{
    x = y = 0;
    altitude = 0;
    length = width = height = 0;

    gravitied = false;
    velocity = stef::Vector3::ZERO;
    acceleration = stef::Vector3::ZERO;
    damping = 0.0f;
    mass = 0.0f;
    elasticity = 0.0f;

    collideType = CollideNon;
    collidable = false;
    triggered = false;
}

void SolidObjInfo::InitAsNormal(stef::real mass, int width)
{
    x = y = 0;
    altitude = 0;
    length = height = 0;
    this->width = width;

    gravitied = true;
    velocity = stef::Vector3::ZERO;
    acceleration = stef::Vector3::ZERO;
    damping = 0.9f;
    this->mass = mass;
    elasticity = 0.9f;

    collideType = CollideA;
    collidable = true;
    triggered = false;
}

void SolidObjInfo::SetPosition(int x, int y, int altitude)
{
    this->x = x;
    this->y = y;
    this->altitude = altitude;
}

SolidObj::SolidObj(const char* name, SolidObjInfo info, const char* imgFile, int textureNum)
    :SolidObj(info, imgFile, textureNum)
{
    this->name = name;
}

SolidObj::SolidObj(SolidObjInfo info)
    :SolidObj(info, NULL, 0)
{
}

SolidObj::SolidObj(SolidObjInfo info, const char* imgFile, int textureNum)
    :DrawableComp(textureNum, imgFile, NULL)    //SolidObj's attached object must be NULL
{
    SetAbsPos(info.x, info.y - info.altitude - info.height / 2 + info.width * 2);

    box.SetPosition(ToMeter(info.x), ToMeter(info.y), ToMeter(info.altitude));

    if(info.width == 0)
    {
        ENG_LogError("Bad arguement: the Width for SolidObj cannot be zero!");
    }
    box.SetHalfSize(stef::Vector3(ToMeter(info.length) / 2,
                                  ToMeter(info.width) / 2,
                                  ToMeter(info.height) / 2));

    box.SetVelocity(info.velocity);

    if(info.gravitied)
        box.SetAcceleration(stef::Vector3::GRAVITY + info.acceleration);
    else
        box.SetAcceleration(info.acceleration);

    box.SetMass(info.mass);
    box.SetDamping(info.damping);
    box.SetElasticity(info.elasticity);

    //collision...
    this->collideType = info.collideType;
    this->collidable = info.collidable;
    this->triggered = info.triggered;

    box.ClearAccumulator();

    tag = tagUnknown;
    box.fatherObj = this;
}

SolidObj::~SolidObj()
{
}

void SolidObj::Encollidable()
{
    collidable = true;
}

void SolidObj::Discollidable()
{
    collidable = false;
}

void SolidObj::ToggleCollidability()
{
    if(collidable)
        collidable = false;
    else
        collidable = true;
}

bool SolidObj::GetCollidable()
{
    return collidable;
}

void SolidObj::SetTriggered(bool triggered)
{
    this->triggered = triggered;
}

bool SolidObj::GetTriggered()
{
    return triggered;
}

SceneBox& SolidObj::GetBox()
{
    return box;
}

Tag SolidObj::GetTag()
{
    return tag;
}

void SolidObj::SetTag(Tag tag)
{
    this->tag = tag;
}

DBG_Status SolidObj::InitInScene(Scene *scene)
{
    DBG_Status status = DBG_OK;

    status |= DrawableComp::InitInScene(scene);
    if(status & DBG_REP_OPR)
        return status;

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

DBG_Status SolidObj::DumpOutOfScene()
{
    DBG_Status status = DBG_OK;

    if(motherScene)
    {
        motherScene->solidObjs.remove(this);
        motherScene->pWorld.GetParticles().remove(&box);
    }

    status |= DrawableComp::DumpOutOfScene();
    return status;
}

DBG_Status SolidObj::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= DrawableComp::Update(deltTick);

//    SetAbsPos(ToPix(box.GetPosition().x),
//              ToPix(box.GetPosition().y - box.GetPosition().z));

    return status;
}

DBG_Status SolidObj::Draw(Uint32 deltTick, Camera* camera)
{
    SetAbsPos(ToPix(box.GetPosition().x),
              ToPix(box.GetPosition().y - box.GetPosition().z));

    return DrawableComp::Draw(deltTick, camera);
}

DBG_Status SolidObj::DrawExtra(Uint32 deltTick, Camera* camera)
{
    DBG_Status status = DBG_OK;

    status |= DrawableComp::DrawExtra(deltTick, camera);

    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(motherScene->render, &r, &g, &b, &a);

    int absX, absY;
    GetAbsPos(&absX, &absY);

    SDL_SetRenderDrawColor(motherScene->render, GET_R(GROUND_TOUCH_COLOR), GET_G(GROUND_TOUCH_COLOR), GET_B(GROUND_TOUCH_COLOR), 0xFF);
    DrawGroundTouch(camera);

    if(box.GetAwake())
        SDL_SetRenderDrawColor(motherScene->render,
                               GET_R(COLLIDE_BOX_COLOR),
                               GET_G(COLLIDE_BOX_COLOR),
                               GET_B(COLLIDE_BOX_COLOR), 0xFF);
    else
        SDL_SetRenderDrawColor(motherScene->render,
                               GET_R(SLEEP_COLLIDE_BOX_COLOR),
                               GET_G(SLEEP_COLLIDE_BOX_COLOR),
                               GET_B(SLEEP_COLLIDE_BOX_COLOR), 0xFF);
    DrawCollideBox(camera);

    SDL_SetRenderDrawColor(motherScene->render, r, g, b, a);

    return status;
}

void SolidObj::DrawCollideBox(Camera* camera)
{
    SDL_Rect rect;
    rect.x = ToPix(box.GetPosition().x - box.GetHalfSize().x);
    rect.y = ToPix(box.GetPosition().y - box.GetPosition().z - box.GetHalfSize().z - box.GetHalfSize().y);
    rect.w = ToPix(box.GetHalfSize().x * 2);
    rect.h = ToPix((box.GetHalfSize().y + box.GetHalfSize().z) * 2);
    if(camera)
        rect += *camera;
    SDL_RenderDrawRect(motherScene->render, &rect);
    SDL_RenderDrawLine(motherScene->render, rect.x, rect.y + ToPix(box.GetHalfSize().y * 2),
                                            rect.x + rect.w, rect.y + ToPix(box.GetHalfSize().y * 2));
}

void SolidObj::DrawGroundTouch(Camera* camera)
{
    SDL_Rect rect;
    rect.x = ToPix(box.GetPosition().x - box.GetHalfSize().x);
    rect.y = ToPix(box.GetPosition().y - box.GetHalfSize().y);
    rect.w = ToPix(box.GetHalfSize().x * 2);
    rect.h = ToPix(box.GetHalfSize().y * 2);
    if(camera)
        rect += *camera;
    SDL_RenderDrawRect(motherScene->render, &rect);

    SDL_Point points[2];
    points[0].x = ToPix(box.GetPosition().x) - 5;
    points[0].y = ToPix(box.GetPosition().y);
    points[1].x = ToPix(box.GetPosition().x) + 5;
    points[1].y = ToPix(box.GetPosition().y);
    if(camera)
    {
        points[0] += *camera;
        points[1] += *camera;
    }
    SDL_RenderDrawLines(motherScene->render, points, 2);

    points[0].x = ToPix(box.GetPosition().x);
    points[0].y = ToPix(box.GetPosition().y) - 5;
    points[1].x = ToPix(box.GetPosition().x);
    points[1].y = ToPix(box.GetPosition().y) + 5;
    if(camera)
    {
        points[0] += *camera;
        points[1] += *camera;
    }
    SDL_RenderDrawLines(motherScene->render, points, 2);
}

}
