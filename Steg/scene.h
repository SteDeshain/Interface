//status.h
//created by SteDeshain
//created on 2016/08/03
//latest updated on 2016/08/03
//The game scene class in SteDeshain engine driven by SDL2.0
//a scene should be registered in "Game::stack<stack> scenes"

#ifndef __SCENE_H__
#define __SCENE_H__

#include <list>
#include "SDL2/SDL.h"
#include "status.h"
#include "camera.h"
#include "pworld.h"

namespace steg
{

class Game;
class GameComp;
class DrawableComp;
class GUI;
class SolidObj;
class InputHandler;
class GUIOperateHandler;
class Scene;

class CollisionDetector: public stef::ParticleContactGenerator
{

protected:
    Scene* scene;

public:
    void Init(Scene* scene);

    unsigned virtual AddContact(stef::ParticleContact* contact, unsigned limit) const;

};

class Scene
{
    friend class Game;

public:
	SDL_Renderer* render;
	Camera* camera;
	InputHandler* inputHandler;
	GUIOperateHandler* guiOperateHandler;
	stef::ParticleWorld pWorld;    //stef physics world
	stef::GroundContactsWithBox groundContactGenerator;
	stef::SceneWallsContactsWithBox wallContactGenerator;
	CollisionDetector collisionDetector;
	unsigned maxSolidObjs;

	int mapWidthNum, mapHeightNum;  //tiles num of the scene map size
                                    //default ground tiles' top-left should be (0, 0)

	Scene(int mapWidthNum, int mapHeightNum, SDL_Renderer* render);
	virtual ~Scene();

	virtual DBG_Status InitScene() = 0;
	virtual DBG_Status HandleEvent(SDL_Event event);
	virtual DBG_Status HandleInput();
	virtual DBG_Status Update(Uint32 deltTick);
	virtual DBG_Status UpdatePhysics();
	virtual DBG_Status Draw(Uint32 deltTick);
	virtual DBG_Status DrawExtra(Uint32 deltTick);

	virtual DBG_Status DetectOverlap(); //overlaps detect and coarse collide detect and resort solid objects

	DBG_Status ResortGUIs();
	virtual DBG_Status ResortSolidObjs();   //temp: no virtual
	DBG_Status ResortSceneComps();

	//debug: show Scene information
//	virtual DBG_Status DrawSceneInfo(Uint32 deltTick, Camera* camera);

	Scene& operator<<(GameComp* comp);
	Scene& operator>>(GameComp* comp);

//protected:
	std::list<GameComp*> comps;

	std::list<DrawableComp*> drawableComps;
	std::list<GUI*> GUIComps;
	GUI* selectedGUIComp;
	std::list<SolidObj*> solidObjs;

private:
    int phyLastStartTick;
    int phyStartTick;

};

}

#endif	//__SCENE_H__
