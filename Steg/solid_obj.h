//solid_h.h
//created by SteDeshain
//created on 2016/08/15
//latest updated on 2016/08/13
//SolidObj class inherited from DrawablwComp, main objects in game scene

#ifndef __SOLID_OBJ_H__
#define __SOLID_OBJ_H__

#include "SDL2/SDL.h"
#include "drawable_comp.h"
#include "collide_type.h"
#include "box.h"
#include "core.h"
#include <list>

namespace steg
{

enum Tag
{
    tagUnknown = 0,
    tagConstructure,
    tagPlayer,
    tagFriend,
    tagEnemy,
    tagNPC,
    tagInteractivable,
    tagSenseBox,
};

struct SolidObjInfo
{
    int x, y;       //pixel position of the obj center
    int altitude;   //pixel height from bottom to the ground where z == 0
    int length;     //pixel size in x axis, be 0 for auto volume
    int width;      //pixel size in y axis, cannot be 0
    int height;     //pixel size in z axis, be 0 for auto volume

    bool gravitied;             //whether it is effected by gravity
    stef::Vector3 velocity;
    stef::Vector3 acceleration;
    stef::real damping;
    stef::real mass;            //negative or REAL_MAX for infinite mass
    stef::real elasticity;      //bounciness from 0.0(absolute soft) to 1.0(no bounciness)

    CollideType collideType;
    bool collidable;
    bool triggered;

    void Clear();
    void InitAsNormal(stef::real mass, int width);

    void SetPosition(int x, int y, int altitude);
};

class SceneBox: public stef::Box
{

    friend class SolidObj;

public:
    std::list<SolidObj*>& GetContactObjs()
    {
        return contactObjs;
    }

protected:
    SolidObj* fatherObj;

    std::list<SolidObj*> contactObjs;

};

class SolidObj: virtual public DrawableComp
{
    friend class Scene;
    friend class CollisionDetector;

public:
    SolidObj(SolidObjInfo info);    //invisible object
    SolidObj(SolidObjInfo info, const char* imgFile, int textureNum = 1);   //visible
    ~SolidObj();

    void Encollidable();
    void Discollidable();
    void ToggleCollidability();
    bool GetCollidable();

    bool GetTriggered();
    void SetTriggered(bool truggered);

    SceneBox& GetBox();

    int drawLayer;  //order to draw to a scene(necessary???)

    Tag GetTag();
    void SetTag(Tag tag);

protected:
    virtual DBG_Status InitInScene(Scene *scene);
    virtual DBG_Status DumpOutOfScene();

    virtual DBG_Status Update(Uint32 deltTick);
    virtual DBG_Status Draw(Uint32 deltTick, Camera* camera);

    //debug
    virtual DBG_Status DrawExtra(Uint32 deltTick, Camera* camera);
    virtual void DrawCollideBox(Camera* camera);
    virtual void DrawGroundTouch(Camera* camera);

    SceneBox box;
    CollideType collideType;
    bool collidable;
    bool triggered;

    Tag tag;

//temp
public:
    std::list<SolidObj*> frontObjs;
    std::list<SolidObj*> checkedObjs;

};

}

#endif  // __SOLID_OBJ_H__
