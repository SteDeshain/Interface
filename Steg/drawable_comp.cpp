#include "drawable_comp.h"
#include "color.h"
#include <iostream>
#include "gui.h"

namespace steg
{

DrawableComp::DrawableComp(unsigned textureNum, const char* imgFiles, DrawableComp* attachedPlatform)
	:textureNum(textureNum), imgFile(imgFiles), attachedPlatform(attachedPlatform)
{
	currentTexture = NULL;

//	if(imgFiles == NULL || textureNum == 0)
	if(textureNum == 0)// || imgFiles == NULL)
	{
		textures = NULL;
		visible = false;
	}
	else
	{
		textures = new SDL_Texture*[textureNum];
		if(NULL == textures)
		{
            visible = false;
			ENG_LogWarn("Cannot malloc memory for \"SDL_Texture** texture\"!");
        }
        else
        {
            for(int i = 0; i < textureNum; i++)
            {
                textures[i] = NULL;
            }
            visible = true;
        }
	}

	drawErrored = false;

	SetRelativePos();		//set default position as (0, 0)
	SetAngle();
	flipMode = SDL_FLIP_NONE;
}

DrawableComp::DrawableComp(const char* imgFile, DrawableComp* attachedPlatform)
	:DrawableComp(1, imgFile, attachedPlatform)
{

}

DrawableComp::DrawableComp(int x, int y, unsigned int textureNum, const char* imgFiles, DrawableComp* attachedPlatform)
	:DrawableComp(textureNum, imgFiles, attachedPlatform)
{
	SetRelativePos(x, y);		//set default position as (x, y)
}

DrawableComp::DrawableComp(int x, int y, const char* imgFile, DrawableComp* attachedPlatform)
	:DrawableComp(x, y, 1, imgFile, attachedPlatform)
{

}

DrawableComp::~DrawableComp()
{
	if(textures)
	{
        for(int i = 0; i < textureNum; i++)
        {
            if(textures[i])
                SDL_DestroyTexture(textures[i]);
        }
		delete[] textures;
    }
    textures = NULL;
}

void DrawableComp::HideComp()
{
	visible = false;
}

void DrawableComp::ShowComp()
{
	visible = true;
}

void DrawableComp::ToggleVisiblity()
{
    if(visible)
        visible = false;
    else
        visible = true;
}

void DrawableComp::SetMove(int dx, int dy)
{
    x += dx;
    y += dy;
    drawRect = SDL_Rect{x - drawRect.w / 2, y - drawRect.h / 2, drawRect.w, drawRect.h};
	UpdateDestRect();
}

void DrawableComp::SetMove(SDL_Point mov)
{
    SetMove(mov.x, mov.y);
}

bool DrawableComp::IsVisible()
{
    return visible;
}

void DrawableComp::SetVisible(bool visible)
{
    this->visible = visible;
}

//set and get relative coordination to attachedPlatform
void DrawableComp::SetRelativePos(int x, int y)
{
	this->x = x;
	this->y = y;
	drawRect = SDL_Rect{x - drawRect.w / 2, y - drawRect.h / 2, drawRect.w, drawRect.h};
	UpdateDestRect();
}

void DrawableComp::SetRelativePos(SDL_Point point)
{
	x = point.x;
	y = point.y;
	drawRect = SDL_Rect{x - drawRect.w / 2, y - drawRect.h / 2, drawRect.w, drawRect.h};
	UpdateDestRect();
}

void DrawableComp::SetRelativeLeftTop(int x, int y)
{
    this->x = x + drawRect.w / 2;
    this->y = y + drawRect.h / 2;
    drawRect = SDL_Rect{x, y, drawRect.w, drawRect.h};
	UpdateDestRect();
}

void DrawableComp::SetRelativeLeftTop(SDL_Point point)
{
    SetRelativeLeftTop(point.x, point.y);
}

void DrawableComp::SetRelativeLeft(int left)
{
    SetRelativeLeftTop(left, this->y - drawRect.h / 2);
}

void DrawableComp::SetRelativeRight(int right)
{
    SetRelativeLeftTop(right - drawRect.w, this->y - drawRect.h / 2);
}

void DrawableComp::SetRelativeTop(int top)
{
    SetRelativeLeftTop(this->x - drawRect.w / 2, top);
}

void DrawableComp::SetRelativeBottom(int bottom)
{
    SetRelativeLeftTop(this->x - drawRect.w / 2, bottom - drawRect.h);
}

SDL_Point DrawableComp::GetRelativePos(int *x, int* y)
{
    if(x)
        *x = this->x;
    if(y)
        *y = this->y;

	return (SDL_Point){this->x, this->y};
}

SDL_Point DrawableComp::GetRelativeLeftTop(int* x, int* y)
{
    int left = this->x - drawRect.w / 2;
    int top = this->y - drawRect.h / 2;

    if(x)
        *x = left;
    if(y)
        *y = top;

    return (SDL_Point){left, top};
}

SDL_Point DrawableComp::GetRelativeRightBottom(int* x, int* y)
{
    int right = this->x + drawRect.w / 2;
    int bottom = this->y + drawRect.h / 2;

    if(x)
        *x = right;
    if(y)
        *y = bottom;

    return (SDL_Point){right, bottom};
}

//set and get absolute coordination to scene map
void DrawableComp::SetAbsPos(int absX, int absY)
{
    //temp
    SetAbsPos(SDL_Point{absX, absY});

//    //temp
//    if(NULL == attachedPlatform)
//        SetRelativePos(absX, absY);
//    else
//    {
//        //temp: need a better way?
////        SDL_Point originAbsPos = GetAbsPos();
////        int dy = originAbsPos.y - absY;
////        int dx = originAbsPos.x - absX;
////        SetRelativePos(x - dx, y - dy);
//    }
}

void DrawableComp::SetAbsPos(SDL_Point absPoint)
{
    //temp
    if(NULL == attachedPlatform)
        SetRelativePos(absPoint.x, absPoint.y);
    else
    {
#if 0
        //temp: need a better way?
        SDL_Point originAbsPos = GetAbsPos();
        int dy = originAbsPos.y - absPoint.y;
        int dx = originAbsPos.x - absPoint.x;
        SetRelativePos(x - dx, y - dy);
#else
        //a better way
        DrawableComp* currentPlatform = attachedPlatform;
        std::list<DrawableComp*> platformsList;
        while(currentPlatform)
        {
            platformsList.push_front(currentPlatform);
            currentPlatform = currentPlatform->attachedPlatform;
        }
        SDL_Point currentPos = absPoint;
        SDL_Point platformLeftTop;
        Canvas* currentCanv = NULL;
        for(std::list<DrawableComp*>::iterator p = platformsList.begin();
            p != platformsList.end();
            p++)
        {
            //apply to each platform
            platformLeftTop = (*p)->GetRelativeLeftTop();
            if(currentCanv = dynamic_cast<Canvas*>(*p))
            {
                //apply the scroll effect
                SDL_Point canvScroll = currentCanv->GetScrollOffset();
                platformLeftTop.x -= canvScroll.x;
                platformLeftTop.y -= canvScroll.y;
            }
            currentPos.x -= platformLeftTop.x;
            currentPos.y -= platformLeftTop.y;
        }
        //get the final relative position
        SetRelativePos(currentPos);
#endif // 0
    }
}

SDL_Point DrawableComp::GetAbsPos(int* absX, int* absY)
{
    int currentX = x;
    int currentY = y;
    DrawableComp* currentAttached = attachedPlatform;
    while(NULL != currentAttached)
    {
        currentX += currentAttached->drawRect.x;
        currentY += currentAttached->drawRect.y;

        currentAttached = currentAttached->attachedPlatform;
    }

    if(absX)
        *absX = currentX;
    if(absY)
        *absY = currentY;

    return SDL_Point{currentX, currentY};
}

void DrawableComp::SetAbsLeftTop(int absLeft, int absTop)
{
    int absX = absLeft + drawRect.w / 2;
    int absY = absTop + drawRect.h / 2;
    SetAbsPos(absX, absY);
}

void DrawableComp::SetAbsLeftTop(SDL_Point absLeftTop)
{
    int absX = absLeftTop.x + drawRect.w / 2;
    int absY = absLeftTop.y + drawRect.h / 2;
    SetAbsPos(absX, absY);
}

SDL_Point DrawableComp::GetAbsLeftTop(int* absLeft, int* absTop)
{
    SDL_Point center = GetAbsPos();
    int left = center.x - drawRect.w / 2;
    int top = center.y - drawRect.h / 2;
    if(absLeft)
        *absLeft = left;
    if(absTop)
        *absTop = top;

    return SDL_Point{left, top};
}

//GetAbsDestRect() should use the destRect to calculate
SDL_Rect DrawableComp::GetAbsDestRect(int* x, int* y, int* w, int* h)
{
//    int absX, absY;
//    GetAbsPos(&absX, &absY);

    int currentX = destRect.x;
    int currentY = destRect.y;
    DrawableComp* currentAttached = attachedPlatform;
    while(NULL != currentAttached)
    {
        currentX += currentAttached->drawRect.x;
        currentY += currentAttached->drawRect.y;

        currentAttached = currentAttached->attachedPlatform;
    }

    if(x)
        *x = currentX;
    if(y)
        *y = currentY;
    if(w)
        *w = destRect.w;
    if(h)
        *h = destRect.h;

    return SDL_Rect{currentX, currentY, destRect.w, destRect.h};
}

void DrawableComp::SetSourceRect(SDL_Rect sourceRect)
{
	this->sourceRect = sourceRect;
}

void DrawableComp::SetSourceSize(int w, int h)
{
    sourceRect.w = w;
    sourceRect.h = h;
}

void DrawableComp::SetRelativeDrawRect(SDL_Rect drawRect)
{
	this->drawRect = drawRect;
	x = drawRect.x + drawRect.w / 2;
	y = drawRect.y + drawRect.h / 2;
	UpdateDestRect();
}

SDL_Rect DrawableComp::GetRelativeDrawRect()
{
    return drawRect;
}

SDL_Rect DrawableComp::GetAbsDrawRect()
{
    if(attachedPlatform == NULL)
        return drawRect;
    else
    {

    }
}

void DrawableComp::SetDrawSize(int w, int h)
{
	drawRect.w = w;
	drawRect.h = h;
	drawRect.x = x - w / 2;
	drawRect.y = y - h / 2;
	UpdateDestRect();
}

void DrawableComp::SetAngle(double angle)
{
    this->angle = angle;
}

DrawableComp* DrawableComp::GetAttachedPlatform()
{
    return attachedPlatform;
}

void DrawableComp::UpdateDestRect()
{
    destRect = drawRect;
}

DBG_Status DrawableComp::Draw(Uint32 deltTick, Camera* camera)
{
	DBG_Status status = DBG_OK;

	SDL_Rect tempRect = GetAbsDestRect();       //get absolute coordination
	if(camera)
        tempRect += *camera;                    //apply to camera
	//temp
	if(currentTexture)
	{
        if(SDL_RenderCopyEx(motherScene->render,	//should use SDL_RenderCopyEx()
                            currentTexture,
                            &sourceRect,
                            &tempRect,
                            angle,
                            NULL,   //temp here
                            flipMode)
          && !drawErrored)
        {
            ENG_LogErrors("Cannot do RenderCopy in DrawableComp", SDL_GetError());
            SDL_ClearError();
            status |= DBG_SDL_ERR;
            drawErrored = true;
        }
	}

	return status;
}

void DrawableComp::ResetErrors()
{
	GameComp::ResetErrors();

	drawErrored = false;
}

//SplitString() function code from http://blog.csdn.net/glt3953/article/details/11115485
static std::vector<std::string> SplitString(std::string str, std::string pattern)
{
	std::vector<std::string> result;
	std::string::size_type pos;
	str += pattern;		//扩展字符串以方便操作
	int size = str.size();

	for(int i = 0; i < size; i++)
	{
		pos=str.find(pattern, i);
		if(pos < size)
		{
			std::string s = str.substr(i, pos-i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

DBG_Status DrawableComp::InitInScene(Scene* scene)
{
	DBG_Status status = DBG_OK;

	status |= GameComp::InitInScene(scene);

	if(status & DBG_REP_OPR)
        return status;

	if(imgFile == NULL || textureNum == 0)
	{
        visible = false;
//        textures = NULL;  //can't do this
        currentTexture = NULL;
		SetSourceSize();
		SetDrawSize();
        return status;
	}

	status |= LoadTexture(scene);

    scene->drawableComps.push_back(this);

	return status;
}

DBG_Status DrawableComp::LoadTexture(Scene* scene)
{
    DBG_Status status = DBG_OK;

	using std::string;
	using std::vector;

	//load textures
	vector<string> files = SplitString(string(imgFile), string("|,"));
	//for(vector<string>::iterator it = files.begin(); it != files.end(); it++)
	for(int i = 0; i < files.size() && i < textureNum; i++)
	{
		if(NULL == (textures[i] = GetTexture(scene->render, files[i].c_str())))
		{
			ENG_LogError("Cannot load textures for DrawableComp!");
			status |= DBG_SDL_ERR | DBG_IMG_ERR;
		}
	}
	currentTexture = textures[0];

	//init sourceRect
	//sourceRect has the same size with default texture as default
	sourceRect.x = sourceRect.y = 0;
	int w, h;
	if(SDL_QueryTexture(currentTexture, NULL, NULL, &w, &h))
	{
		ENG_LogError(SDL_GetError());
		SDL_ClearError();
		SetSourceSize();
		SetDrawSize();
	}
	else
	{
        SetSourceSize(w, h);
		SetDrawSize(w, h);
	}
	//SetRelativePos(x, y);		//unknown bug, must call SetPos(x, y) here, otherwise the coordination will be (0, 0)
                                //for some unknown reason too, this bug disappear

    return status;
}

DBG_Status DrawableComp::DumpOutOfScene()
{
	DBG_Status status = DBG_OK;

	currentTexture = NULL;
	for(int i = 0; i < textureNum; i++)
	{
        if(textures[i])
        {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
	}

//	SetSourceSize();
//	SetDrawSize();

    if(motherScene)
    {
        motherScene->drawableComps.remove(this);
    }

	status |= GameComp::DumpOutOfScene();
    return status;
}

//debug
DBG_Status DrawableComp::DrawExtra(Uint32 deltTick, Camera* camera)
{
    DBG_Status status = DBG_OK;

    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(motherScene->render, &r, &g, &b, &a);

    int absX, absY;
    GetAbsPos(&absX, &absY);

    SDL_SetRenderDrawColor(motherScene->render, GET_R(DRAW_RECT_COLOR), GET_G(DRAW_RECT_COLOR), GET_B(DRAW_RECT_COLOR), 0xFF);
    DrawDrawRect(absX, absY, camera);
    SDL_SetRenderDrawColor(motherScene->render, GET_R(CENTER_COLOR), GET_G(CENTER_COLOR), GET_B(CENTER_COLOR), 0xFF);
    DrawCenter(absX, absY, camera);

    SDL_SetRenderDrawColor(motherScene->render, r, g, b, a);

    return status;
}

void DrawableComp::DrawDrawRect(int absX, int absY, Camera* camera)
{
    SDL_Rect absDrawRect = SDL_Rect{absX - drawRect.w / 2,
                                    absY - drawRect.h / 2,
                                    drawRect.w,
                                    drawRect.h};
    if(camera)
        absDrawRect += *camera;
    SDL_RenderDrawRect(motherScene->render, &absDrawRect);
}

void DrawableComp::DrawCenter(int absX, int absY, Camera* camera)
{
    SDL_Point vertPoints[2];
    vertPoints[0] = SDL_Point{absX - 5, absY - 5};
    vertPoints[1] = SDL_Point{absX + 5, absY + 5};
    SDL_Point HorizPoints[2];
    HorizPoints[0] = SDL_Point{absX - 5, absY + 5};
    HorizPoints[1] = SDL_Point{absX + 5, absY - 5};
    if(camera)
    {
        vertPoints[0] += *camera;
        vertPoints[1] += *camera;
        HorizPoints[0] += *camera;
        HorizPoints[1] += *camera;
    }
    SDL_RenderDrawLines(motherScene->render, vertPoints, 2);
    SDL_RenderDrawLines(motherScene->render, HorizPoints, 2);
}

}
