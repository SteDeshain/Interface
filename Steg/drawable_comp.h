//drawable_comp.h
//created by SteDeshain
//created on 2016/08/06
//latest updated on 2016/08/06
//DrawableComp inherited from GameComp could be drawed in Scene
//DrawableComp is initialized only after inserted into a Scene by "Scene << GameComp*"

#ifndef __DRAWABLE_COMP_H__
#define __DRAWABLE_COMP_H__

#include <vector>
#include <string>
#include "SDL2/SDL.h"
#include "game_comp.h"
#include "sources_load.h"
#include "log.h"
#include "scene.h"
#include "camera.h"

namespace steg
{

class DrawableComp: public GameComp
{
	friend class Scene;

public:
	DrawableComp(unsigned textureNum, const char* imgFiles = NULL, DrawableComp* attachedPlatform = NULL);		//DrawableComp(3, "a.png|b.bmp|c.jpg");
	DrawableComp(const char* imgFile = NULL, DrawableComp* attachedPlatform = NULL);
	DrawableComp(int x, int y, unsigned textureNum, const char* imgFiles = NULL, DrawableComp* attachedPlatform = NULL);
	DrawableComp(int x, int y, const char* imgFile = NULL, DrawableComp* attachedPlatform = NULL);
	virtual ~DrawableComp();

	void HideComp();
	void ShowComp();
	void ToggleVisiblity();
	void SetMove(int dx = 0, int dy = 0);
	bool IsVisible();

	//set and get relative coordination to attachedPlatform
	void SetRelativePos(int x = 0, int y = 0);
	void SetRelativePos(SDL_Point point);
	void SetRelativeLeftTop(int x = 0, int y = 0);
	void SetRelativeLeftTop(SDL_Point point);
	SDL_Point GetRelativePos(int* x = NULL, int* y = NULL);

	//set and get absolute coordination to scene map
	void SetAbsPos(int absX = 0, int absY = 0);         //
	void SetAbsPos(SDL_Point absPoint);                 //they actualy set the relative x and y
	SDL_Point GetAbsPos(int* absX = NULL, int* absY = NULL);    //return SDL_point value as absPos, and assign it to *x and *y

	//set source and draw rect
	void SetRelativeDrawRect(SDL_Rect drawRect);    //not usually used actually, often use SetRelativePos() and SetDrawSize() instead
	SDL_Rect GetRelativeDrawRect();
	SDL_Rect GetAbsDrawRect();
	void SetDrawSize(int w = 0, int h = 0);
	virtual void SetSourceRect(SDL_Rect sourceRect);
	virtual void SetSourceSize(int w = 0, int h = 0);

	void SetAngle(double angle = 0.0f);

protected:
    DrawableComp* attachedPlatform; //attached platform as DrawableComp, for GUI may need attachedPlatform too
                                    //to calculate the final coordination, use this->x, y and attachedPlatform->drawRect
	int textureNum;
	const char* imgFile;
	SDL_Texture* currentTexture;	//textures中的其中一个指针引用
	SDL_Texture** textures;			//DrawableComp真正的SDL_Texture*的所在地
	bool visible;
	bool drawErrored;				//to set or reset it to keep it not to log errors in every draw frame

//About the coordination system
	//in game, allways use the relative coordination x and y
	//destRect is also bounded to the relative x and y
	//when we need absolute coordination, just calculate it with relative pos and attachedPlatform
	//like when an enemy on a platform tries to attack the player on aonther platform
	//we need both absolute coordinations of enemy and player
	int x, y;						//position coordination in attachedPlatform, at the center of this->destRect
	//int absX, absY;               //absolute coordination in the scene map, after calculated with attachedPlatform
                                    //mustn't set value of absX and absY, which can only be read
                                    //so I made it unavailable
	SDL_Rect sourceRect;
	SDL_Rect destRect;
	SDL_Rect drawRect;              //real rectangle drawn on screen, for one-destRect DrawableComp, it's the same as destRect
                                    //if you want to set drawRect, you must use functions like SetDrawRect()
                                    //in which way, the x, y and destRect will be updated at the same time
                                    //and it's the same if you want to set x or y
                                    //this varity is designed also for self DrawableComp as a attachedPlatform
                                    //drawRect is an interface to access destRect

    double angle;
	SDL_Point rotateCenter;
	SDL_RendererFlip flipMode;

	virtual DBG_Status Draw(Uint32 deltTick, Camera* camera);

	virtual void ResetErrors();
	virtual DBG_Status InitInScene(Scene* scene);	//called after inserted into a Scene
	virtual DBG_Status LoadTexture(Scene* scene);   //inner use in InitInScene()
	virtual DBG_Status DumpOutOfScene();			//called after thrown out by a Scene

	virtual void UpdateDestRect();  //after modifying x, y or destRect, must call this function to make drawRect valid

	//debug
	virtual DBG_Status DrawExtra(Uint32 deltTick, Camera* camera);
	void DrawDrawRect(int absX, int absY, Camera* camera);
	void DrawCenter(int absX, int absY, Camera* camera);

protected:
    virtual SDL_Rect GetAbsDestRect(int* x = NULL, int* y = NULL, int* w = NULL, int* h = NULL);
    //it's easy to get abs drawRect with absX and absY, so we don't need GetAbsDrawRect() function

};

}

#endif	//__DRAWABLE_COMP_H__
