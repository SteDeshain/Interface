#include "scene.h"
#include "game.h"
#include "drawable_comp.h"
#include "gui.h"
#include "solid_obj.h"
#include "label.h"
#include "event_handler.h"
#include "collision_detector.h"

namespace steg
{

void CollisionDetector::Init(Scene* scene)
{
    this->scene = scene;
}

//must be called every frame
unsigned CollisionDetector::AddContact(stef::ParticleContact* contact, unsigned limit) const
{
    //coarse collide
    scene->DetectOverlap();

    unsigned int collideCounts = 0;

    for(std::list<SolidObj*>::iterator s = scene->solidObjs.begin();
        s != scene->solidObjs.end();
        s++)
    {
        if(!((*s)->collidable))
            continue;

        for(std::list<SolidObj*>::iterator t = (*s)->frontObjs.begin();
            t != (*s)->frontObjs.end();
            t++)
        {
            if(!((*t)->collidable) || !IsCollided((*s)->collideType, (*t)->collideType))
                continue;

            //detect collision
            SceneBox& sBox = (*s)->GetBox();
            SceneBox& tBox = (*t)->GetBox();
            CollideInfo info = CollisionDetect(sBox, tBox);
            if(info.collideNormal == stef::Vector3::ZERO)
            {
                //no collision
            }
            else
            {
                //add contact boxes
                bool sHasT = false;
                bool tHasS = false;
                std::list<SolidObj*>& sContactObjs = (*s)->GetBox().GetContactObjs();
                std::list<SolidObj*>& tContactObjs = (*t)->GetBox().GetContactObjs();
                for(std::list<SolidObj*>::iterator it = sContactObjs.begin();
                    it != sContactObjs.end();
                    it++)
                {
                    if((*t) == (*it))
                    {
                        sHasT = true;
                        break;
                    }
                }
                for(std::list<SolidObj*>::iterator it = tContactObjs.begin();
                    it != tContactObjs.end();
                    it++)
                {
                    if((*s) == (*it))
                    {
                        tHasS = true;
                        break;
                    }
                }
                if(!sHasT)
                    sContactObjs.push_back(*t);
                if(!tHasS)
                    tContactObjs.push_back(*s);

                if(!((*s)->GetTriggered()) && !((*t)->GetTriggered()))
                {
                    //add contact
                    contact->contactNormal = info.collideNormal;
                    contact->particle[0] = &((*s)->GetBox());
                    contact->particle[1] = &((*t)->GetBox());
                    contact->penetration = info.penetration;
                    contact->restitution = (*s)->GetBox().GetElasticity() * (*t)->GetBox().GetElasticity();
                    //temp: friction...

                    contact++;
                    collideCounts++;
                }

                if(collideCounts >= limit)
                    return collideCounts;
            }
        }
    }

    return collideCounts;
}

Scene::Scene(int mapWidthNum, int mapHeightNum, SDL_Renderer* render)
	:mapWidthNum(mapWidthNum), mapHeightNum(mapHeightNum), render(render),
	 maxSolidObjs(1000), pWorld(10000), groundContactGenerator(0.0f)    //temp
{
    camera = new Camera(Game::windowWidth, Game::windowHeight, mapWidthNum, mapHeightNum);
    if(NULL == camera)
        ENG_LogError("cannot create Camera object for Scene!");

    inputHandler = new InputHandler();
    if(NULL == inputHandler)
        ENG_LogError("cannot create InputHandler object for Scene!");

    guiOperateHandler = new GUIOperateHandler(this);
    if(NULL == guiOperateHandler)
        ENG_LogError("cannot create GUIOperateHandler object for Scene!");

    phyLastStartTick = 0;

    //groundContactGenerator and pWorld share the same &Particles
    groundContactGenerator.Init(&pWorld.GetParticles());
    pWorld.GetContactGenerators().push_back(&groundContactGenerator);

    wallContactGenerator.Init(&pWorld.GetParticles());
    wallContactGenerator.SetWalls(0, ToMeter(800), 0, ToMeter(480));    //temp
    pWorld.GetContactGenerators().push_back(&wallContactGenerator);

    collisionDetector.Init(this);
    pWorld.GetContactGenerators().push_back(&collisionDetector);

    selectedGUIComp = NULL;
}

Scene::~Scene()
{
    if(camera)
        delete camera;
    camera = NULL;  //not necessary actually

    if(inputHandler)
        delete inputHandler;
    inputHandler = NULL;

    //temp
    //需要协调delete world时把残留的body释放了,与下面释放残留的solidObj中的body时的冲突
    //delete all GameComps still in Scene
    for(std::list<GameComp*>::iterator it = comps.begin(); it != comps.end(); it++)
    {
        delete *it;
    }
}

DBG_Status Scene::HandleEvent(SDL_Event event)
{
	DBG_Status status = DBG_OK;

	if(event.user.type == CustomEventsType[CUSTOM_SCENEEVENT])
    {
        if(event.user.code == evcResortSolidObjs && event.user.data1 == this)
        {
            return ResortSolidObjs();
        }
    }

	for(std::list<GameComp*>::iterator it = comps.begin(); it != comps.end(); it++)
	{
        status |= (*it)->HandleEvent(event);
	}

	return status;
}

DBG_Status Scene::HandleInput()
{
    DBG_Status status = DBG_OK;

    inputHandler->Update(0);
    guiOperateHandler->Update(0);

    return status;
}

DBG_Status Scene::Update(Uint32 deltTick)
{
	DBG_Status status = DBG_OK;

	for(std::list<GameComp*>::iterator it = comps.begin(); it != comps.end(); it++)
	{
        status |= (*it)->Update(deltTick);
	}

	return status;
}

DBG_Status Scene::UpdatePhysics(Uint32 reminingTick)
{
#if 0
    phyStepCount = 0;

    Uint32 tickA = 0;
    int remins = reminingTick;

    if(0 == phyLastStartTick)
    {
        phyLastStartTick = SDL_GetTicks();
        return DBG_OK;
    }

    do
    {
        tickA = SDL_GetTicks();

        phyStepCount++;

        //temp: should be another place like at the beginning of a frame
        pWorld.StartFrame();
        //temp
        for(std::list<SolidObj*>::iterator s = solidObjs.begin();
            s != solidObjs.end();
            s++)
        {
            (*s)->GetBox().GetContactObjs().clear();
            //???...need good way
        }

        //run physics
        stef::real duration = (stef::real)(SDL_GetTicks() - phyLastStartTick) * 0.001f;
        phyLastStartTick = SDL_GetTicks();
        pWorld.RunPhysics(duration);

        remins -= SDL_GetTicks() - tickA;
    } while(remins > 0);
#else
    if(0 == phyLastStartTick)
    {
        phyLastStartTick = SDL_GetTicks();
        return DBG_OK;
    }
    phyStartTick = SDL_GetTicks();

    //for debug
    if(phyStartTick - phyLastStartTick > TICKS_PER_FRAME)
    {
        phyStartTick = phyLastStartTick + TICKS_PER_FRAME;
    }

    pWorld.StartFrame();

    for(std::list<SolidObj*>::iterator s = solidObjs.begin();
        s != solidObjs.end();
        s++)
    {
        (*s)->GetBox().GetContactObjs().clear();
        //???...need good way
    }

    stef::real duration = (stef::real)(phyStartTick - phyLastStartTick) * 0.001f;
    pWorld.RunPhysics(duration);
#endif
    return DBG_OK;
}

static bool BoxesAbove(const stef::Box& a, const stef::Box& b)
{
    //return true if a is above b, false otherwise

    stef::real aBottom = a.GetBottom();
    stef::real aTop = a.GetTop();
    stef::real aFront = a.GetFront();
    stef::real aBack = a.GetBack();
    stef::real aLeft = a.GetLeft();
    stef::real aRight = a.GetRight();

    stef::real bBottom = b.GetBottom();
    stef::real bTop = b.GetTop();
    stef::real bFront = b.GetFront();
    stef::real bBack = b.GetBack();
    stef::real bLeft = b.GetLeft();
    stef::real bRight = b.GetRight();

    //no penetration
    if(aBottom >= bTop || aBack >= bFront)
        return true;
    else if(aTop < bBottom || aFront < bBack)
        return false;

    //there is penetration
    if(aFront > bFront)
        return true;
    else
        return false;

    return false;
}

DBG_Status Scene::DetectOverlap()
{
	DBG_Status status = DBG_OK;

	bool needToResort = false;

	//clear checkedObjs
	for(std::list<SolidObj*>::iterator s = solidObjs.begin(); s != solidObjs.end(); s++)
    {
        (*s)->checkedObjs.clear();
    }

	for(std::list<SolidObj*>::iterator s = solidObjs.begin(); s != solidObjs.end(); s++)
    {
        SDL_Rect sRect = (*s)->GetAbsDrawRect();
        for(std::list<SolidObj*>::iterator t = solidObjs.begin(); t != solidObjs.end(); t++)
        {
            if(s == t || (*s) == (*t))
                continue;

            SDL_Rect tRect = (*t)->GetAbsDrawRect();
            if(SDL_HasIntersection(&sRect, &tRect) == SDL_FALSE)
            {
                //remove t from s->frontObjs and remove s from t->frontObjs
                //there is no necessary to resort
                (*s)->frontObjs.remove(*t);
                (*t)->frontObjs.remove(*s);
            }
            else
            {
                bool checked = false;
                for(std::list<SolidObj*>::iterator it = (*s)->checkedObjs.begin();
                    it != (*s)->checkedObjs.end();
                    it++)
                {
                    if((*it) == (*t))
                    {
                        checked = true;
                        break;
                    }
                }

                if(checked)
                {
                    continue;
                }
                else
                {
                    (*s)->checkedObjs.push_back(*t);
                    (*t)->checkedObjs.push_back(*s);

                    bool alreadyInFront = false;
//                    if(BoxesAbove((*s)->GetBox(), (*t)->GetBox()))
                    if((*s)->GetBox().GetFront() > (*t)->GetBox().GetFront())
                    {
                        for(std::list<SolidObj*>::iterator it = (*t)->frontObjs.begin();
                            it != (*t)->frontObjs.end();
                            it++)
                        {
                            if((*it) == (*s))
                            {
                                alreadyInFront = true;
                                break;
                            }
                        }
                        if(!alreadyInFront)
                        {
                            (*s)->frontObjs.remove(*t);
                            (*t)->frontObjs.push_back(*s);
                            needToResort = true;
                        }
                    }
                    else
                    {
                        for(std::list<SolidObj*>::iterator it = (*s)->frontObjs.begin();
                            it != (*s)->frontObjs.end();
                            it++)
                        {
                            if((*it) == (*t))
                            {
                                alreadyInFront = true;
                                break;
                            }
                        }
                        if(!alreadyInFront)
                        {
                            (*t)->frontObjs.remove(*s);
                            (*s)->frontObjs.push_back(*t);
                            needToResort = true;
                        }
                    }
                }
            }
        }
    }

    if(needToResort)
    {
        //push event
        PushResortSolidObjsEvent(this);
    }

	return status;
}

DBG_Status Scene::Draw(Uint32 deltTick)
{
	DBG_Status status = DBG_OK;

	//temp
	for(std::list<DrawableComp*>::iterator it = drawableComps.begin(); it != drawableComps.end(); it++)
	{
		if((*it)->visible)
		{
			status |= (*it)->Draw(deltTick, camera);
		}
	}

	for(std::list<SolidObj*>::iterator it = solidObjs.begin(); it != solidObjs.end(); it++)
	{
		if((*it)->visible)
		{
			status |= (*it)->Draw(deltTick, camera);
		}
	}

	for(std::list<GUI*>::iterator it = GUIComps.begin(); it != GUIComps.end(); it++)
	{
		if((*it)->visible)
		{
			status |= (*it)->Draw(deltTick, NULL);
		}
	}

	return status;
}

DBG_Status Scene::DrawExtra(Uint32 deltTick)
{
	DBG_Status status = DBG_OK;

	//temp
	for(std::list<DrawableComp*>::iterator it = drawableComps.begin(); it != drawableComps.end(); it++)
	{
		if((*it)->visible)
		{
			status |= (*it)->DrawExtra(deltTick, camera);
		}
	}

	for(std::list<SolidObj*>::iterator it = solidObjs.begin(); it != solidObjs.end(); it++)
	{
		if((*it)->visible)
		{
			status |= (*it)->DrawExtra(deltTick, camera);
		}
	}
#ifdef SHOW_UI_EXTRA
    for(std::list<GUI*>::iterator it = GUIComps.begin(); it != GUIComps.end(); it++)
	{
		if((*it)->visible)
		{
			status |= (*it)->DrawExtra(deltTick, NULL);
		}
	}
#endif // SHOW_UI_EXTRA
	return status;
}

//DBG_Status Scene::DrawSceneInfo(Uint32 deltTick, Camera* camera)
//{
//    DBG_Status status = DBG_OK;
//
//    return status;
//}

Scene& Scene::operator<<(GameComp* comp)
{
	comp->InitInScene(this);

	return *this;
}

Scene& Scene::operator>>(GameComp* comp)
{
    comps.remove(comp);

    GUI* gComp = NULL;
	SolidObj* sComp = NULL;
    DrawableComp* dComp = NULL;
    if(gComp = dynamic_cast<GUI*>(comp))
	{
        GUIComps.remove(gComp);
	}
	else if(sComp = dynamic_cast<SolidObj*>(comp))
	{
        solidObjs.remove(sComp);
        //temp
        //need to remove body from world
        pWorld.GetParticles().remove(&sComp->GetBox());
	}
	//temp
	else if(dComp = dynamic_cast<DrawableComp*>(comp))
	{
        drawableComps.remove(dComp);
	}
	else
	{
        ENG_LogError("Unsupproted GameComp class attempts to be removed out of Scene!");
	}

	comp->DumpOutOfScene();

	return *this;
}

static bool CompareSolidObjs(SolidObj* a, SolidObj* b)
{
//    for(std::list<SolidObj*>::iterator it = a->frontObjs.begin();
//        it != a->frontObjs.end();
//        it++)
//    {
//        if((*it) == b)
//            return true;
//    }
//
//    for(std::list<SolidObj*>::iterator it = b->frontObjs.begin();
//        it != b->frontObjs.end();
//        it++)
//    {
//        if((*it) == a)
//            return false;
//    }
//
//    return true;

    //use frontObj list to resort always get wrong sort
    //so we use a simpler and faster way
    if(a->GetBox().GetBottom() >= b->GetBox().GetTop())
        return false;

    if(a->GetBox().GetFront() < b->GetBox().GetFront())
        return true;
    else if(a->GetBox().GetFront() == b->GetBox().GetFront())
    {
        if(a->GetBox().GetTop() < b->GetBox().GetTop())
            return true;
    }
    else
        return false;
}

DBG_Status Scene::ResortSolidObjs()
{
    DBG_Status status = DBG_OK;

    solidObjs.sort(CompareSolidObjs);

    return status;
}

}
