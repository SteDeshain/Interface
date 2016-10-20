#include "camera.h"
#include "game.h"

namespace steg
{

Camera::Camera(int w, int h, int mapWidthNum, int mapHeightNum)
    :mapWidthNum(mapWidthNum), mapHeightNum(mapHeightNum)
{
	viewRect.w = w;
	viewRect.h = h;
	SetLeftTop();	//set default viewRect left-top as (0, 0)
}

Camera::~Camera()
{

}

void Camera::Restrict()
{
#ifdef RELEASE
    if(Game::tileWidth * mapWidthNum > viewRect.w)
    {
        if(viewRect.x < 0)
            viewRect.x = 0;

        if(viewRect.x > Game::tileWidth * mapWidthNum - viewRect.w)
            viewRect.x = Game::tileWidth * mapWidthNum - viewRect.w;
    }
    else
    {
        viewRect.x = (Game::tileWidth * mapWidthNum - viewRect.w) / 2;
    }

    if(Game::tileWidth * mapHeightNum > viewRect.h)
    {
        if(viewRect.y < 0)
            viewRect.y = 0;

        if(viewRect.y > Game::tileWidth * mapHeightNum - viewRect.h)
            viewRect.y = Game::tileWidth * mapHeightNum - viewRect.h;
    }
    else
    {
        viewRect.y = (Game::tileWidth * mapHeightNum - viewRect.h) / 2;
    }
#endif // RELEASE
}

void Camera::SetLeftTop(int x, int y)
{
	viewRect.x = x;
	viewRect.y = y;
	Restrict();
}

void Camera::SetCenter(int x, int y)
{
	viewRect.x = x - viewRect.w / 2;
	viewRect.y = y - viewRect.h / 2;
	Restrict();
}

void Camera::SetMove(int dx, int dy)
{
    viewRect.x += dx;
    viewRect.y += dy;
	Restrict();
}

SDL_Point Camera::GetLeftTop(int* x, int* y)
{
    if(x)
        *x = viewRect.x;
    if(y)
        *y = viewRect.y;

    return SDL_Point{viewRect.x, viewRect.y};
}

//SDL_Point
SDL_Point Camera::operator+(SDL_Point originPoint) const
{
	return SDL_Point{originPoint.x - viewRect.x, originPoint.y - viewRect.y};
}

SDL_Point operator+(const SDL_Point& originPoint, const Camera& camera)
{
	return camera + originPoint;
}

SDL_Point& operator+=(SDL_Point& originPoint, const Camera& camera)
{
	originPoint = camera + originPoint;
	return originPoint;
}

//SDL_Rect
SDL_Rect Camera::operator+(SDL_Rect originRect) const
{
	return SDL_Rect{originRect.x - viewRect.x, originRect.y - viewRect.y, originRect.w, originRect.h};
}

SDL_Rect operator+(const SDL_Rect& originRect, const Camera& camera)
{
	return camera + originRect;
}

SDL_Rect& operator+=(SDL_Rect& originRect, const Camera& camera)
{
	originRect = camera + originRect;
	return originRect;
}

//SDL_Point
//...

}
