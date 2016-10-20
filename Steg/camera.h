//camera.h
//created by SteDeshain
//created on 2016/08/08
//latest updated on 2016/08/08
//2D camera class

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "SDL2/SDL.h"
#include "status.h"

namespace steg
{

class Camera
{
public:
	Camera(int w, int h, int mapWidthNum, int mapHeightNum);
	virtual ~Camera();

	void SetLeftTop(int x = 0, int y = 0);
	void SetCenter(int x = 0, int y = 0);
	void SetMove(int dx = 0, int dy = 0);

	SDL_Point GetLeftTop(int* x = NULL, int* y = NULL);

	SDL_Rect operator+(SDL_Rect originRect) const;
	friend SDL_Rect operator+(const SDL_Rect& originRect, const Camera& camera);
	friend SDL_Rect& operator+=(SDL_Rect& originRect, const Camera& camera);

	SDL_Point operator+(SDL_Point originPoint) const;
	friend SDL_Point operator+(const SDL_Point& originPoint, const Camera& camera);
	friend SDL_Point& operator+=(SDL_Point& originPoint, const Camera& camera);

protected:
	SDL_Rect viewRect;
	int mapWidthNum, mapHeightNum;

	void Restrict();    //to keep the camera in scene map

private:
};

}

#endif	//__CAMERA_H__
