#include "gui.h"
#include "sources_load.h"
#include "color.h"
#include "event_handler.h"

namespace steg
{

GUI::GUI(int x, int y, int textureNum, const char* imgFile, Canvas* canvas)
    :DrawableComp(x, y, textureNum, imgFile, canvas)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
    extraTrans = 1.0f;
}

GUI::GUI(int x, int y, int drawIndex, Canvas* canvas)
    :DrawableComp(x, y, 1, NULL, canvas), drawIndex(drawIndex)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
    extraTrans = 1.0f;
}

GUI::GUI(int textureNum, const char* imgFile, Canvas* canvas)
    :DrawableComp(textureNum, imgFile, canvas)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
    extraTrans = 1.0f;
}

GUI::~GUI()
{

}

bool GUI::IsSelectable()
{
    return selectable;
}

bool GUI::IsSelected()
{
    if(!selectable)
        return false;

    return selected;
}

void GUI::SetExtraTrans(float extraTrans)
{
    if(extraTrans < 0)
        this->extraTrans = 0.0f;
    else if(extraTrans > 1)
        this->extraTrans = 1.0f;
    else
        this->extraTrans = extraTrans;
}
float GUI::GetExtraTrans()
{
    return extraTrans;
}

DBG_Status GUI::HandleEvent(SDL_Event event)
{
    DBG_Status status = DBG_OK;

    if(event.user.type == CustomEventsType[CUSTOM_GUIEVENT])
    {
        if(event.user.code == evcSelectGUIComp && event.user.data1 == this)
        {
            if(selectable)
            {
                selected = true;
                motherScene->selectedGUIComp = this;
                //temp
//                SDL_SetTextureAlphaMod(currentTexture, 200);
            }
        }
        else if(event.user.code == evcUnSelectGUIComp && event.user.data1 == this)
        {
            selected = false;
            motherScene->selectedGUIComp = NULL;
            //temp
//            SDL_SetTextureAlphaMod(currentTexture, 255);
        }
    }

    return status;
}

DBG_Status GUI::InitInScene(Scene *scene)
{
    DBG_Status status = DBG_OK;

    status |= DrawableComp::InitInScene(scene);
    if(status & DBG_REP_OPR)
        return status;

    scene->drawableComps.remove(this);

    entireSize = SDL_Point{sourceRect.w, sourceRect.h};
    drawSize = SDL_Point{drawRect.w, drawRect.h};

    scene->GUIComps.push_back(this);

    SDL_GetTextureAlphaMod(currentTexture, &textureAlpha);
//    textureAlpha = 255;

    return status;
}

//DBG_Status GUI::LoadTexture()
//{
//    entireSize = SDL_Point{sourceRect.w, sourceRect.h};
//}

DBG_Status GUI::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= DrawableComp::Update(deltTick);

    float finalTrans = (float)textureAlpha / 255.0f * extraTrans;

    //follow canvas
    Canvas* canvas = dynamic_cast<Canvas*>(attachedPlatform);
    if(canvas)
    {
//        visible = canvas->IsVisible();
        finalTrans *= canvas->extraTrans;
        //if this is a canvas, addapt extraTrans to motherCanvas' extraTrans
        Canvas* canvasThis = dynamic_cast<Canvas*>(this);
        if(canvasThis)
        {
            extraTrans = canvasThis->originExtraTrans * canvas->extraTrans;
            if(canvas->IsVisible())
            {
                visible = canvasThis->originVisible;
            }
            else
            {
                visible = false;
            }
        }
        else
        {
            visible = canvas->IsVisible();
        }
    }

    SDL_SetTextureAlphaMod(currentTexture, 255.0f * finalTrans);

    if(!visible && motherScene->selectedGUIComp == this)
    {
        motherScene->selectedGUIComp = NULL;
    }

    return status;
}

SDL_Rect GUI::GetAbsDestRect(int* x, int* y, int* w, int* h)
{
    SDL_Rect currentDstRect = SDL_Rect{this->x - drawSize.x / 2,
                                       this->y - drawSize.y / 2,
                                       drawSize.x,
                                       drawSize.y};

    SDL_Rect currentSrcRect = SDL_Rect{sourceStartPoint.x, sourceStartPoint.y, entireSize.x, entireSize.y};
    currentSrcRect = CutSrcRect(currentSrcRect, drawSize);

    Canvas* currentCanvas = dynamic_cast<Canvas*>(attachedPlatform);
    while(currentCanvas)
    {
        //...
        //to calculate the currentSrcRect and the current destRect
        currentDstRect.x -= currentCanvas->scrollOffset.x;  //attention: use minus "-="
        currentDstRect.y -= currentCanvas->scrollOffset.y;
        currentDstRect.x += currentCanvas->x - currentCanvas->drawSize.x / 2;
        currentDstRect.y += currentCanvas->y - currentCanvas->drawSize.y / 2;
        //cut destRect
        int leftCut = currentCanvas->x - currentCanvas->drawSize.x / 2 - currentDstRect.x;
        int topCut = currentCanvas->y - currentCanvas->drawSize.y / 2 - currentDstRect.y;
        SDL_Rect canvasDrawRect = SDL_Rect{currentCanvas->x - currentCanvas->drawSize.x / 2,
                                           currentCanvas->y - currentCanvas->drawSize.y / 2,
                                           currentCanvas->drawSize.x,
                                           currentCanvas->drawSize.y};
        SDL_bool hasIntersect = SDL_IntersectRect(&currentDstRect,
                                                  &canvasDrawRect,
                                                  &currentDstRect);

        //cut sourcRect
        if(hasIntersect == SDL_TRUE)
        {
            currentSrcRect.w = currentDstRect.w;
            currentSrcRect.h = currentDstRect.h;
            if(leftCut > 0)
                currentSrcRect.x += leftCut;
            if(topCut > 0)
                currentSrcRect.y += topCut;
        }

        if(currentDstRect.w <= 0 || currentDstRect.h <= 0)
            break;

        //to avoid other DrawableComps being as a Canvas
        currentCanvas = dynamic_cast<Canvas*>(currentCanvas->attachedPlatform);
    }

    SetSourceRect(currentSrcRect);  //set the sourceRect to the correct value

    if(x)
        *x = currentDstRect.x;
    if(y)
        *y = currentDstRect.y;
    if(w)
        *w = currentDstRect.w;
    if(h)
        *h = currentDstRect.h;
    return currentDstRect;
}

SDL_Rect GUI::CutSrcRect(SDL_Rect srcRect, SDL_Point destSize)
{
    return srcRect;
}

//Canvas state machine functions
static DBG_Status ShowUpdate(Uint32 deltTick, Canvas* canvas, CanvasState* self)
{
    DBG_Status status = DBG_OK;

    canvas->SetVisible(true);
    if(canvas->GetSlideInfo() == NULL)
    {
        canvas->SetExtraTrans(1.0f);
        canvas->SetCanvasState(Canvas::StateEnumIdle);
    }
    else
    {
        self->totalMove += deltTick * (canvas->GetSlideInfo())->movePerTick;
        SDL_Point newPos = canvas->GetSlideInfo()->destPos;
        switch((canvas->GetSlideInfo())->showWay)
        {
        case SlideRight:
            newPos.x += (canvas->GetSlideInfo())->slideDistance;
            newPos.x -= self->totalMove;
            break;

        case SlideLeft:
            newPos.x -= (canvas->GetSlideInfo())->slideDistance;
            newPos.x += self->totalMove;
            break;

        case SlideDown:
            newPos.y += (canvas->GetSlideInfo())->slideDistance;
            newPos.y -= self->totalMove;
            break;

        case SlideUp:
        default:
            newPos.y -= (canvas->GetSlideInfo())->slideDistance;
            newPos.y += self->totalMove;
            break;
        }
        canvas->SetRelativePos(newPos);

        canvas->AddExtraTrans(deltTick * (canvas->GetSlideInfo())->deltTransPerTick);
        if(canvas->GetExtraTrans() >= 1.0f)    //temp: should judge by position rather than transparency(necessary)
        {
            canvas->SetExtraTrans(1.0f);
            canvas->SetRelativePos(canvas->GetSlideInfo()->destPos);
            canvas->SetCanvasState(Canvas::StateEnumIdle);

            self->totalMove = 0.0f;
        }
    }

    return status;
}
//static DBG_Status ShowHandleEvent(SDL_Event event, Canvas* canvas, CanvasState* self)
//{
//    return DBG_OK;
//}

//static DBG_Status IdleUpdate(Uint32 deltTick, Canvas* canvas, CanvasState* self)
//{
//    return DBG_OK;
//}
static DBG_Status IdleHandleEvent(SDL_Event event, Canvas* canvas, CanvasState* self)
{
    DBG_Status status = DBG_OK;

    if(event.user.type == CustomEventsType[CUSTOM_GUIEVENT])
    {
        DrawableComp* motherCanvas = canvas->GetAttachedPlatform();
        if(!motherCanvas ||
           (motherCanvas && motherCanvas->IsVisible()))
        {
            if(event.user.code == evcShowCanvas && event.user.data1 == canvas)
            {
//                if(!canvas->GetOriginVisible())
                if(!canvas->IsVisible())
                {
                    canvas->SetCanvasState(Canvas::StateEnumShow);
                    canvas->SetExtraTrans(0.0f);
                }
            }
            else if(event.user.code == evcHideCanvas && event.user.data1 == canvas)
            {
//                if(canvas->GetOriginVisible())
                if(canvas->IsVisible())
                {
                    canvas->SetCanvasState(Canvas::StateEnumHide);
                }
            }
        }
    }

    return status;
}

static DBG_Status HideUpdate(Uint32 deltTick, Canvas* canvas, CanvasState* self)
{
    DBG_Status status = DBG_OK;

    if(canvas->GetSlideInfo() == NULL)
    {
        canvas->SetVisible(false);
        canvas->SetCanvasState(Canvas::StateEnumIdle);
    }
    else
    {
        self->totalMove += deltTick * (canvas->GetSlideInfo())->movePerTick;
        SDL_Point newPos = canvas->GetSlideInfo()->destPos;
        switch((canvas->GetSlideInfo())->hideWay)
        {
        case SlideLeft:
            newPos.x -= self->totalMove;
            break;

        case SlideRight:
            newPos.x += self->totalMove;
            break;

        case SlideUp:
            newPos.y -= self->totalMove;
            break;

        case SlideDown:
        default:
            newPos.y += self->totalMove;
            break;
        }
        canvas->SetRelativePos(newPos);

        canvas->AddExtraTrans(deltTick * (canvas->GetSlideInfo())->deltTransPerTick * -1);
        if(canvas->GetExtraTrans() <= 0.0f)
        {
            canvas->SetVisible(false);
            canvas->SetRelativePos(canvas->GetSlideInfo()->destPos);
            canvas->SetCanvasState(Canvas::StateEnumIdle);

            self->totalMove = 0.0f;
        }
    }

    return status;
}
//static DBG_Status HideHandleEvent(SDL_Event event, Canvas* canvas, CanvasState* self)
//{
//    return DBG_OK;
//}

Canvas::Canvas(SDL_Color color, float transparency, SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas)
    :GUI(1, NULL, motherCanvas),
     color(color), transparency(transparency)
{
    entireSize = canvasSize;
    drawSize = SDL_Point{viewRect.w, viewRect.h};
    SetRelativeDrawRect(viewRect);
    scrollOffset = SDL_Point{0, 0};
    slideInfo = NULL;
    originExtraTrans = extraTrans;
    originVisible = visible;

    //state machine
    canvasShow.HandleEvent = NULL;
    canvasShow.Update = ShowUpdate;
    canvasShow.totalMove = 0.0f;

    canvasIdle.HandleEvent = IdleHandleEvent;
    canvasIdle.Update = NULL;       //do nothing
    canvasIdle.totalMove = 0.0f;

    canvasHide.HandleEvent = NULL;
    canvasHide.Update = HideUpdate;
    canvasHide.totalMove = 0.0f;

    currentState = &canvasIdle;      //default as Idle state(already shown)
}

Canvas::Canvas(const char* imgFile, float transparency, SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas)
    :GUI(1, imgFile, motherCanvas),
     color(GetColor(Black)), transparency(transparency)
{
    entireSize = canvasSize;
    drawSize = SDL_Point{viewRect.w, viewRect.h};
    SetRelativeDrawRect(viewRect);
    scrollOffset = SDL_Point{0, 0};
    slideInfo = NULL;
    originExtraTrans = extraTrans;
    originVisible = visible;

    //state machine
    canvasShow.HandleEvent = NULL;
    canvasShow.Update = ShowUpdate;
    canvasShow.totalMove = 0.0f;

    canvasIdle.HandleEvent = IdleHandleEvent;
    canvasIdle.Update = NULL;       //do nothing
    canvasIdle.totalMove = 0.0f;

    canvasHide.HandleEvent = NULL;
    canvasHide.Update = HideUpdate;
    canvasHide.totalMove = 0.0f;

    currentState = &canvasIdle;      //default as Idle state(already shown)
}

Canvas::~Canvas()
{
    if(slideInfo)
    {
        delete slideInfo;
        slideInfo = NULL;
    }
}

void Canvas::SetScrollOffset(int dx, int dy)
{
    scrollOffset.x += dx;
    if(scrollOffset.x < 0)
        scrollOffset.x = 0;
    else if(scrollOffset.x > entireSize.x - drawSize.x)
        scrollOffset.x = entireSize.x - drawSize.x;

    scrollOffset.y += dy;
    if(scrollOffset.y < 0)
        scrollOffset.y = 0;
    else if(scrollOffset.y > entireSize.y - drawSize.y)
        scrollOffset.y = entireSize.y - drawSize.y;
}

static void ResetStateDestPos(Canvas* canvas)
{
//    canvas->GetState(Canvas::StateEnumHide)->destPosition = canvas->GetRelativePos();
//    canvas->GetState(Canvas::StateEnumIdle)->destPosition = canvas->GetRelativePos();
//    canvas->GetState(Canvas::StateEnumShow)->destPosition = canvas->GetRelativePos();

//    if(canvas->GetSlideInfo())
//    {
//        canvas->GetSlideInfo()->destPos = canvas->GetRelativePos();
//    }
}

void Canvas::SetSlideInfo(SlideWay showWay, SlideWay hideWay, Uint32 slideTicks, unsigned int slideDistance)
{
    slideInfo = new SlideInfo;
    if(slideInfo)
    {
        slideInfo->showWay = showWay;
        slideInfo->hideWay = hideWay;
        slideInfo->slideTicks = slideTicks;
        slideInfo->slideDistance = slideDistance;

        slideInfo->deltTransPerTick = 1.0f / (float)slideTicks;
        slideInfo->movePerTick = (float)slideDistance / (float)slideTicks;
        slideInfo->destPos = GetRelativePos();
    }
    else
    {
        ENG_LogError("Cannot allocate SlideInfo for Canvas.");
    }
}
void Canvas::SetSlideInfo()
{
    this->SetSlideInfo(SlideUp, SlideDown, 200, 10);
}
Canvas::SlideInfo* Canvas::GetSlideInfo()
{
    return this->slideInfo;
}

void Canvas::SetCanvasState(CanvasStateEnum state)
{
    switch(state)
    {
    case StateEnumHide:
        currentState = &canvasHide;
        break;

    case StateEnumShow:
        currentState = &canvasShow;
        break;

    case StateEnumIdle:
    default:
        currentState = &canvasIdle;
        break;
    }
}

void Canvas::SetExtraTrans(float extraTrans)
{
    GUI::SetExtraTrans(extraTrans);

    originExtraTrans = this->extraTrans;
}
float Canvas::GetExtraTrans()
{
    return originExtraTrans;
}
void Canvas::AddExtraTrans(float deltaTrans)
{
    extraTrans += deltaTrans;
    if(extraTrans < 0)
        extraTrans = 0.0f;
    else if(extraTrans > 1)
        extraTrans = 1.0f;

    originExtraTrans = extraTrans;
}

//bool Canvas::IsVisible()
//{
////    return originVisible;
//    return visible;
//}
void Canvas::SetVisible(bool visible)
{
    this->visible = visible;
    this->originVisible = visible;
}

void Canvas::SetRelativePos(int x, int y)
{
    GUI::SetRelativePos(x, y);

    ResetStateDestPos(this);
}

void Canvas::SetRelativePos(SDL_Point point)
{
    GUI::SetRelativePos(point);

    ResetStateDestPos(this);
}

void Canvas::SetRelativeLeftTop(int x, int y)
{
    GUI::SetRelativeLeftTop(x, y);

    ResetStateDestPos(this);
}

void Canvas::SetRelativeLeftTop(SDL_Point point)
{
    GUI::SetRelativeLeftTop(point);

    ResetStateDestPos(this);
}

CanvasState* Canvas::GetState(CanvasStateEnum stateEnum)
{
    switch(stateEnum)
    {
    case StateEnumShow:
        return &canvasShow;
        break;
    case StateEnumHide:
        return &canvasHide;
        break;
    case StateEnumIdle:
    default:
        return &canvasIdle;
        break;
    }
}

bool Canvas::GetOriginVisible()
{
    return originVisible;
}

DBG_Status Canvas::InitInScene(Scene* scene)
{
    DBG_Status status = DBG_OK;

	status |= GameComp::InitInScene(scene);

	if(status & DBG_REP_OPR)
        return status;

    SDL_Rect originDrawRect = drawRect;
    if(imgFile == NULL)
    {
        //create color texture
        if(entireSize.x < drawSize.x)
            entireSize.x = drawSize.x;
        if(entireSize.y < drawSize.y)
            entireSize.y = drawSize.y;

        textures[0] = GetColorTexture(scene->render, entireSize, color, transparency);
        currentTexture = textures[0];
    }
    else
    {
        //create picture texture
        DrawableComp::LoadTexture(scene); //this function will change the drawRect, so I restored it earlier
        SDL_SetTextureAlphaMod(textures[0], (Uint8)(transparency * 255));
        //一切大小紧跟texture
        SDL_QueryTexture(currentTexture, NULL, NULL, &(entireSize.x), &(entireSize.y));
        if(entireSize.x < drawSize.x)
            drawSize.x = entireSize.x;
        if(entireSize.y < drawSize.y)
            drawSize.y = entireSize.y;
    }
    SetRelativeDrawRect(originDrawRect);
    sourceRect.x = sourceRect.y = 0;
    SetSourceSize(drawRect.w, drawRect.h);
    //to set the scroll bars
    scrollOffset = SDL_Point{0, 0};

    scene->GUIComps.push_back(this);

    SDL_GetTextureAlphaMod(currentTexture, &textureAlpha);

    return status;
}

DBG_Status Canvas::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= GUI::Update(deltTick);

    //state machine update
    if(currentState->Update)
        currentState->Update(deltTick, this, currentState);

    return status;
}

DBG_Status Canvas::HandleEvent(SDL_Event event)
{
    DBG_Status status = DBG_OK;

    status |= GUI::HandleEvent(event);

    //state machine event handling
    if(currentState->HandleEvent)
        currentState->HandleEvent(event, this, currentState);

    return status;
}

SDL_Rect Canvas::CutSrcRect(SDL_Rect srcRect, SDL_Point destSize)
{
    SDL_Rect tmpRect = SDL_Rect{scrollOffset.x, scrollOffset.y, destSize.x, destSize.y};
    SDL_IntersectRect(&srcRect,
                      &tmpRect,
                      &srcRect);
    return srcRect;
}

Button::Button(int x, int y, SDL_Color color, SDL_Point buttonSize, Canvas* motherCanvas)
    :GUI(x, y, 1, NULL, motherCanvas), color(color)
{
    selectable = true;
    entireSize = buttonSize;
    drawSize = buttonSize;
    SetRelativeDrawRect(SDL_Rect{x - buttonSize.x / 2,
                                 y - buttonSize.y / 2,
                                 buttonSize.x,
                                 buttonSize.y});
}

DBG_Status Button::InitInScene(Scene* scene)
{
    DBG_Status status = DBG_OK;

    status |= GameComp::InitInScene(scene);
    if(status & DBG_REP_OPR)
        return status;

    SDL_Rect originDrawRect = drawRect;
    if(imgFile == NULL)
    {
        SDL_Texture* originTarget = SDL_GetRenderTarget(scene->render);
        SDL_Color originDrawColor;
        SDL_GetRenderDrawColor(scene->render,
                               &(originDrawColor.r),
                               &(originDrawColor.g),
                               &(originDrawColor.b),
                               &(originDrawColor.a));

        SDL_Texture* finTexture = SDL_CreateTexture(scene->render,
                                                    SDL_PIXELFORMAT_RGBA8888,
                                                    SDL_TEXTUREACCESS_TARGET,
                                                    entireSize.x * 2,
                                                    entireSize.y);

        SDL_SetTextureBlendMode(finTexture, SDL_BLENDMODE_BLEND);

        SDL_SetRenderTarget(scene->render, finTexture);

        //draw unselected button
        SDL_Color selectedColor = SDL_Color{color.r < 70 ? 0 : color.r - 70,
                                            color.g < 70 ? 0 : color.g - 70,
                                            color.b < 70 ? 0 : color.b - 70,
                                            color.a};
        SDL_SetRenderDrawColor(scene->render, selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
        SDL_RenderClear(scene->render);

        //draw selected button
//        SDL_Rect selectedRect = SDL_Rect{entireSize.x, 0, entireSize.x, entireSize.y};
        SDL_Rect rect1 = SDL_Rect{0, 0, entireSize.x - 2, entireSize.y - 2};
        SDL_Rect rect2 = SDL_Rect{entireSize.x, 0, entireSize.x - 2, 2};
        SDL_Rect rect3 = SDL_Rect{entireSize.x, 0, 2, entireSize.y - 2};
        SDL_SetRenderDrawColor(scene->render, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(scene->render, &(rect1));
        SDL_RenderFillRect(scene->render, &(rect2));
        SDL_RenderFillRect(scene->render, &(rect3));

        SDL_SetRenderDrawColor(scene->render,
                               originDrawColor.r,
                               originDrawColor.g,
                               originDrawColor.b,
                               originDrawColor.a);
        SDL_SetRenderTarget(scene->render, originTarget);

        textures[0] = finTexture;
        currentTexture = textures[0];
    }
    else
    {
        //picture texture
        DrawableComp::LoadTexture(scene);
        //一切大小紧跟texture
        int sizeX, sizeY;
        SDL_QueryTexture(currentTexture, NULL, NULL, &sizeX, &sizeY);
        sizeX /= 2;
        entireSize = SDL_Point{sizeX / 2, sizeY};
        drawSize = entireSize;
    }
    SetRelativeDrawRect(originDrawRect);
    sourceRect.x = sourceRect.y = 0;
    SetSourceSize(drawRect.w, drawRect.h);

    scene->GUIComps.push_back(this);

    SDL_GetTextureAlphaMod(currentTexture, &textureAlpha);

    return status;
}

DBG_Status Button::HandleEvent(SDL_Event event)
{
    DBG_Status status = DBG_OK;

    status |= GUI::HandleEvent(event);
    if(event.user.type == CustomEventsType[CUSTOM_GUIEVENT])
    {
        if(event.user.code == evcPressedButton && event.user.data1 == this)
        {
            OnButtonPressed();
        }
        else if(event.user.code == evcReleasedButton && event.user.data1 == this)
        {
            OnButtonReleased();
        }
        else if(event.user.code == evcUnSelectGUIComp && event.user.data1 == this)
        {
            //temp
//            SDL_SetTextureAlphaMod(currentTexture, 255);
            sourceStartPoint.x = 0;
        }
    }

    return status;
}

DBG_Status Button::OnButtonPressed()
{
    DBG_Status status = DBG_OK;

    //temp
//    SDL_SetTextureAlphaMod(currentTexture, 200);
    sourceStartPoint.x = entireSize.x;

    return status;
}

DBG_Status Button::OnButtonReleased()
{
    DBG_Status status = DBG_OK;

    //temp
//    SDL_SetTextureAlphaMod(currentTexture, 255);
    sourceStartPoint.x = 0;

    //push some events

    return status;
}

}
