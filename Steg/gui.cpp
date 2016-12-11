#include "gui.h"
#include "sources_load.h"
#include "color.h"
#include "event_handler.h"
#include "game.h"
//tmp
#include "stel.h"

namespace steg
{

GUI::GUI(int x, int y, int textureNum, const char* imgFile, Canvas* canvas)
    :DrawableComp(x, y, textureNum, imgFile, canvas)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
    extraTrans = 1.0f;
    followingCanvas = canvas;
}

GUI::GUI(int x, int y, int drawIndex, Canvas* canvas)
    :DrawableComp(x, y, 1, NULL, canvas), drawIndex(drawIndex)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
    extraTrans = 1.0f;
    followingCanvas = canvas;
}

GUI::GUI(int textureNum, const char* imgFile, Canvas* canvas)
    :DrawableComp(textureNum, imgFile, canvas)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
    extraTrans = 1.0f;
    followingCanvas = canvas;
}

//color texture
GUI::GUI(int x, int y, SDL_Point picSize, SDL_Color color, float transparency, Canvas* canvas)
    :DrawableComp(x, y, 1, NULL, canvas), colorTextureSize(picSize), colorTextureTransparency(transparency)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
    extraTrans = 1.0f;
    followingCanvas = canvas;

    this->color = new SDL_Color;
    if(this->color == NULL)
    {
        ENG_LogError("Cannot malloc SDL_Color structure for GUI in color texture mode.");
    }
    else
    {
        *(this->color) = color;
    }
}

GUI::GUI(const char* name, int x, int y, int drawIndex,
         int textureNum, const char* imgFile, steg::Canvas* canvas)
    :GUI(x, y, textureNum, imgFile, canvas)
{
    this->drawIndex = drawIndex;
    this->name = name;
}

GUI::GUI(const char* name, int x, int y, int drawIndex,
         SDL_Point picSize, SDL_Color color, float transparency, steg::Canvas* canvas)
    :GUI(x, y, picSize, color, transparency, canvas)
{
    this->drawIndex = drawIndex;
    this->name = name;
}

GUI::~GUI()
{
    if(color)
    {
        delete color;
        color = NULL;
    }
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

SDL_Point GUI::GetDrawSize()
{
    return drawSize;
}

SDL_Point GUI::GetEntireSize()
{
    return entireSize;
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
                OnSelected();
            }
        }
        else if(event.user.code == evcUnSelectGUIComp && event.user.data1 == this)
        {
            OnUnSelected();
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

    scene->drawableComps.remove(this);  //temp: remove scene's drawableComps list

    //color texture mode
    if(color)
    {
        textures[0] = GetColorTexture(motherScene->render, colorTextureSize, *color, colorTextureTransparency);
        currentTexture = textures[0];

//        entireSize = colorTextureSize;
//        drawSize = colorTextureSize;
        SetDrawSize(colorTextureSize.x, colorTextureSize.y);
        sourceRect.x = sourceRect.y = 0;
        SetSourceSize(colorTextureSize.x, colorTextureSize.y);

        visible = true;
    }

    entireSize = SDL_Point{sourceRect.w, sourceRect.h};
    drawSize = SDL_Point{drawRect.w, drawRect.h};

    scene->GUIComps.push_back(this);

    SDL_GetTextureAlphaMod(currentTexture, &textureAlpha);

    return status;
}

DBG_Status GUI::DumpOutOfScene()
{
    DBG_Status status = DBG_OK;

    if(motherScene)
    {
        motherScene->GUIComps.remove(this);

        if(motherScene->selectedGUIComp == this)
        {
            motherScene->selectedGUIComp = NULL;
        }

        if(motherScene->pressedGUIComp == this)
        {
            motherScene->pressedGUIComp = NULL;
        }
    }

    status |= DrawableComp::DumpOutOfScene();
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
//    Canvas* canvas = dynamic_cast<Canvas*>(attachedPlatform);
    Canvas* canvas = dynamic_cast<Canvas*>(followingCanvas);
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

DBG_Status GUI::OnSelected()
{
    DBG_Status status = DBG_OK;

    selected = true;
    motherScene->selectedGUIComp = this;

    return status;
}

DBG_Status GUI::OnUnSelected()
{
    DBG_Status status = DBG_OK;

    selected = false;
    motherScene->selectedGUIComp = NULL;

    return status;
}

void GUI::ResetFollowingCanvas(Canvas* newFollowingCanvas)
{
    this->followingCanvas = newFollowingCanvas;
}

//Canvas state machine functions
DBG_Status ShowUpdate(Uint32 deltTick, Canvas* canvas, CanvasState* self)
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
        if(canvas->GetExtraTrans() >= 1.0f)    //temp: should judge by position rather than transparency(necessary ???)
        {
            canvas->SetExtraTrans(1.0f);
            canvas->SetRelativePos(canvas->GetSlideInfo()->destPos);
            canvas->SetCanvasState(Canvas::StateEnumIdle);

            self->totalMove = 0.0f;
        }

        //resume the origin scroll bar state
        //and should do this at every frame in animation
        //to avoid the animation influencing the origin state of the scroll bars
//        if(canvas->verticleScrollBar)
//        {
//            canvas->verticleScrollBar->rollBar->SetRelativeTop(canvas->vertScrollBarPos);
//        }
//        if(canvas->horizonScrollBar)
//        {
//            canvas->horizonScrollBar->rollBar->SetRelativeLeft(canvas->horzScrollBarPos);
//        }

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
DBG_Status IdleHandleEvent(SDL_Event event, Canvas* canvas, CanvasState* self)
{
    DBG_Status status = DBG_OK;

    if(event.user.type == CustomEventsType[CUSTOM_GUIEVENT])
    {
        DrawableComp* motherCanvas = canvas->GetAttachedPlatform();
        if(!motherCanvas ||
           (motherCanvas && motherCanvas->IsVisible()))
        {
//            lua_pushlightuserdata(L, canvas);   // +1
//            PLuaSetToGlobal_J("__cur");         // -1
//            void* tmpPtr = NULL;
//            PLuaPushNil();                  // +1
//            PLuaPushFromTable_J("__cur");   // +1
//            PLuaPop(&tmpPtr);               // -1
//            PLuaPop();                      // -1
//            bool b1 = event.user.code == evcShowCanvas;
////            void* c = (void*)(dynamic_cast<steg::Canvas*>(canvas));
////            void* c = (void*)(static_cast<steg::Canvas*>(canvas));
//            void* c = (reinterpret_cast<void*>(canvas));
////            void* c = (void*)(canvas);
//            void* d = (void*)(event.user.data1);
//            bool b2 = d + 4 == c;
            if(event.user.code == evcShowCanvas && event.user.data1 == canvas)
//            if(b1 && b2)
            {
                if(!canvas->IsVisible())
                {
                    canvas->SetCanvasState(Canvas::StateEnumShow);
                    canvas->SetExtraTrans(0.0f);
                }
            }
            else if(event.user.code == evcHideCanvas && event.user.data1 == canvas)
            {
                if(canvas->IsVisible())
                {
                    canvas->SetCanvasState(Canvas::StateEnumHide);
                    //avoid the animation influences the origin state of the scroll bars
//                    if(canvas->verticleScrollBar)
//                    {
//                        canvas->vertScrollBarPos = canvas->verticleScrollBar->rollBar->GetRelativeLeftTop().y;
//                    }
//                    if(canvas->horizonScrollBar)
//                    {
//                        canvas->horzScrollBarPos = canvas->horizonScrollBar->rollBar->GetRelativeLeftTop().x;
//                    }
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

Canvas::Canvas(const char* name, const char* imgFile, float transparency,
               SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible)
    :Canvas(imgFile, transparency, viewRect, canvasSize, motherCanvas)
{
    this->startVisible = startVisible;
    this->name = name;
}

Canvas::Canvas(const char* name, SDL_Color color, float transparency,
               SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible)
    :Canvas(color, transparency, viewRect, canvasSize, motherCanvas)
{
    this->startVisible = startVisible;
    this->name = name;
}

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

    currentState = &canvasIdle;      //default as Idle state(already shown or already hidden)
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
    if(horizonScrollBar)
    {
        //delete horizonScrollBar
        (*motherScene) >> horizonScrollBar;
        delete horizonScrollBar;
    }
    if(verticleScrollBar)
    {
        //delete verticleScrollBar
        (*motherScene) >> verticleScrollBar;
        delete verticleScrollBar;
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

void Canvas::SetScrollHorizon(float xRatio)
{
    int width = entireSize.x - drawSize.x;
    width *= xRatio;
    scrollOffset.x = width;
}

void Canvas::SetScrollVerticle(float yRatio)
{
    int height = entireSize.y - drawSize.y;
    height *= yRatio;
    scrollOffset.y = height;
}

SDL_Point Canvas::GetScrollOffset(int* x, int* y)
{
    if(x)
        *x = scrollOffset.x;
    if(y)
        *y = scrollOffset.y;
    return scrollOffset;
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

ScrollBar* Canvas::GetHorizonScrollBar()
{
    return horizonScrollBar;
}

ScrollBar* Canvas::GetVerticleScrollBar()
{
    return verticleScrollBar;
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

#if 1
    if(horizonScrollBar == NULL)
    {
        if(entireSize.x > drawSize.x)
        {
            PushCreateScrollBarEvent(this, (void*)scrHorizon);
        }
    }
    else
    {
        (*motherScene) << horizonScrollBar;
    }

    if(verticleScrollBar == NULL)
    {
        if(entireSize.y > drawSize.y)
        {
            PushCreateScrollBarEvent(this, (void*)scrVerticle);
        }
    }
    else
    {
        (*motherScene) << verticleScrollBar;
    }
#else
    //whether need scroll bars
    if(entireSize.x > drawSize.x)
    {
        if(horizonScrollBar == NULL)
        {
            PushCreateScrollBarEvent(this, (void*)scrHorizon);
        }
    }
    if(entireSize.y > drawSize.y)
    {
        if(verticleScrollBar == NULL)
        {
            PushCreateScrollBarEvent(this, (void*)scrVerticle);
        }
    }

    //if it aleardy has scroll bars
    if(horizonScrollBar)
    {
        (*motherScene) << horizonScrollBar;
    }
    if(verticleScrollBar)
    {
        (*motherScene) << verticleScrollBar;
    }
#endif

    scene->GUIComps.push_back(this);

    SDL_GetTextureAlphaMod(currentTexture, &textureAlpha);

    SetVisible(startVisible);

    return status;
}

DBG_Status Canvas::DumpOutOfScene()
{
    DBG_Status status = DBG_OK;

    if(horizonScrollBar)
    {
//        horizonScrollBar->DumpOutOfScene();
        (*motherScene) >> horizonScrollBar;
    }
    if(verticleScrollBar)
    {
//        verticleScrollBar->DumpOutOfScene();
        (*motherScene) >> verticleScrollBar;
    }

    status |= GUI::DumpOutOfScene();
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

    if(event.user.type == CustomEventsType[CUSTOM_GUIEVENT])
    {
        if(event.user.code == evcResizeCanvas && event.user.data1 == this)
        {
            SDL_Point* pNewSize = (SDL_Point*)(event.user.data2);   //danger!
            if(pNewSize)
            {
                ResetTexture(*pNewSize);
            }
        }
    }

    return status;
}

DBG_Status Canvas::ResetTexture(const char* newImg)
{
}

DBG_Status Canvas::ResetTexture(SDL_Point newSize)
{
    DBG_Status status = DBG_OK;

    if(imgFile != NULL)
    {
        ENG_LogError("Cannot resize canvas with picture texture.");
        return DBG_ARG_ERR;
    }

    if(newSize.x < drawSize.x)
        newSize.x = drawSize.x;
    if(newSize.y < drawSize.y)
        newSize.y = drawSize.y;

    if(newSize.x == entireSize.x && newSize.y == entireSize.y)
    {
//        ENG_LogInfo("same size!");
        return DBG_OK;
    }

    SDL_Texture* newTex = GetColorTexture(motherScene->render, newSize, color, transparency);
    if(newTex)
    {
        SDL_DestroyTexture(textures[0]);
        textures[0] = newTex;
        currentTexture = textures[0];
        entireSize = newSize;
        status |= DBG_OK;
    }
    else
    {
        //do no change
        status |= DBG_SDL_ERR;
    }

    //whether need scroll bars
    if(entireSize.x > drawSize.x)
    {
        if(horizonScrollBar == NULL)
        {
            PushCreateScrollBarEvent(this, (void*)scrHorizon);
        }
    }
    //whether need to remove scroll bars
    else if(entireSize.x <= drawSize.x)
    {
        if(horizonScrollBar != NULL)
        {
            PushDeleteScrollBarEvent(this, (void*)scrHorizon);
        }
    }

    if(entireSize.y > drawSize.y)
    {
        if(verticleScrollBar == NULL)
        {
            PushCreateScrollBarEvent(this, (void*)scrVerticle);
        }
    }
    else if(entireSize.y <= drawSize.y)
    {
        if(verticleScrollBar != NULL)
        {
            PushDeleteScrollBarEvent(this, (void*)scrVerticle);
        }
    }

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

Button::Button(int x, int y, const char* imgFile, Canvas* motherCanvas)
    :GUI(x, y, 1, imgFile, motherCanvas)
{
    selectable = true;
}

Button::~Button()
{
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
        textures[0] = GetButtonTexture(scene->render, entireSize, color);
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
        entireSize = SDL_Point{sizeX, sizeY};
        drawSize = entireSize;
    }
    SetRelativeDrawRect(originDrawRect);
    sourceRect.x = sourceRect.y = 0;
    SetSourceSize(drawRect.w, drawRect.h);

    scene->GUIComps.push_back(this);

    SDL_GetTextureAlphaMod(currentTexture, &textureAlpha);

    return status;
}

DBG_Status Button::DumpOutOfScene()
{
    DBG_Status status = DBG_OK;

    buttonDown = false;
    sourceStartPoint.x = 0;

    status |= GUI::DumpOutOfScene();
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
        else if(event.user.code == evcDumpButton && event.user.data1 == this)
        {
            OnButtonDumped();
        }
        else if(event.user.code == evcUnSelectGUIComp && event.user.data1 == this)
        {
            sourceStartPoint.x = 0;
        }
    }

    return status;
}

DBG_Status Button::OnButtonPressed()
{
    DBG_Status status = DBG_OK;

    buttonDown = true;
    sourceStartPoint.x = entireSize.x;

    //when to set pressedGUIComp to NULL, should be handled by guiOperatHandler
    motherScene->pressedGUIComp = this;

    return status;
}

DBG_Status Button::OnButtonReleased()
{
    DBG_Status status = DBG_OK;

//    if(buttonDown)    //no more need
//    {
//    }

//    motherScene->pressedGUIComp = NULL;   //now it's handled by guiOperatHandler

    //push some events
    //...

    buttonDown = false;
    sourceStartPoint.x = 0;

    return status;
}

DBG_Status Button::OnButtonDumped()
{
    DBG_Status status = DBG_OK;

    buttonDown = false;
    sourceStartPoint.x = 0;

    motherScene->pressedGUIComp = NULL;

    return status;
}

DBG_Status Button::OnSelected()
{
    DBG_Status status = DBG_OK;

    status |= GUI::OnSelected();

    return status;
}

DBG_Status Button::OnUnSelected()
{
    DBG_Status status = DBG_OK;

    status |= GUI::OnUnSelected();

    buttonDown = false;
    sourceStartPoint.x = 0;

//    if(motherScene->pressedGUIComp == this)
//    {
//        motherScene->pressedGUIComp = NULL;
//    }

    return status;
}

DragButton::DragButton(int x, int y, SDL_Color color, SDL_Point buttonSize, SDL_Rect* area, Canvas* motherCanvas)
    :Button(x, y, color, buttonSize, motherCanvas)//, GUI(x, y, 1, NULL, motherCanvas)
{
    this->area = new SDL_Rect;
    if(this->area == NULL)
        ENG_LogError("Cannot alloc SDL_Rect for DragButton's area.");
    else
    {
        if(area)
        {
            *(this->area) = *area;
        }
        else
        {
            if(motherCanvas)
                *(this->area) = SDL_Rect{0, 0, motherCanvas->GetEntireSize().x, motherCanvas->GetEntireSize().y};
            else
            {
                delete (this->area);
                this->area = NULL;
            }
        }
    }

    sizeAnchor = SDL_Point{0, 0};
}

DragButton::DragButton(int x, int y, const char* imgFile, SDL_Rect* area, Canvas* motherCanvas)
    :Button(x, y, imgFile, motherCanvas)//, GUI(x, y, 1, imgFile, motherCanvas)
{
    this->area = new SDL_Rect;
    if(this->area == NULL)
        ENG_LogError("Cannot alloc SDL_Rect for DragButton's area.");
    else
    {
        if(area)
        {
            *(this->area) = *area;
        }
        else
        {
            if(motherCanvas)
                *(this->area) = SDL_Rect{0, 0, motherCanvas->GetEntireSize().x, motherCanvas->GetEntireSize().y};
            else
            {
                *(this->area) = SDL_Rect{0, 0, Game::windowWidth, Game::windowHeight};
            }
        }
    }

    sizeAnchor = SDL_Point{0, 0};
}

DragButton::~DragButton()
{
    if(area)
        delete area;
}

float DragButton::ReportHorizonRatio()
{
    return float(GetRelativeLeftTop().x - area->x) / float(area->w - drawSize.x);
}

float DragButton::ReportVerticleRatio()
{
    return float(GetRelativeLeftTop().y - area->y) / float(area->h - drawSize.y);
}

void DragButton::SetAreaPos(int x, int y)
{
    if(area)
    {
        area->x = x - area->w / 2;
        area->y = y - area->h / 2;
    }
}

void DragButton::SetAreaLeftTop(int left, int top)
{
    if(area)
    {
        area->x = left;
        area->y = top;
    }
}

DBG_Status DragButton::InitInScene(Scene* scene)
{
    DBG_Status status = DBG_OK;
    status |= Button::InitInScene(scene);
    if(status & DBG_REP_OPR)
        return status;

    //...

    return status;
}

DBG_Status DragButton::DumpOutOfScene()
{
    DBG_Status status = DBG_OK;

    //...

    status |= Button::DumpOutOfScene();
    return status;
}

DBG_Status DragButton::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= GUI::Update(deltTick);

    //follow mouse
//    if(followMouse)
    if(motherScene->pressedGUIComp == this)
    {
//        SetAbsPos(motherScene->inputHandler->GetMousePosition());
        SetAbsLeftTop(motherScene->inputHandler->GetMousePosition().x - sizeAnchor.x,
                      motherScene->inputHandler->GetMousePosition().y - sizeAnchor.y);
    }

    //restrict posiiton
    RestrictPosition();

    return status;
}

void DragButton::RestrictPosition()
{
    if(area == NULL)
        return;

    bool noRestrictVert = false;
    bool noRestrictHorz = false;
    SDL_Point areaCenter = SDL_Point{area->x + area->w / 2, area->y + area->h / 2};

    if(drawSize.x >= area->w)
    {
        SetRelativePos(areaCenter.x, this->y);
        noRestrictHorz = true;
    }
    if(drawSize.y >= area->h)
    {
        SetRelativePos(this->x, areaCenter.y);
        noRestrictVert = true;
    }
    if(noRestrictVert && noRestrictHorz)
        return;

    if(!noRestrictVert)
        RestrictVertical();

    if(!noRestrictHorz)
        RestrictHorizon();
}

void DragButton::RestrictVertical()
{
    int canvTop = area->y;
    int canvBottom = area->y + area->h;

    int selfTop = GetRelativeLeftTop().y;
    int selfBottom = GetRelativeLeftTop().y + drawSize.y;

    if(selfTop < canvTop)
    {
        SetRelativeTop(canvTop);
    }
    else if(selfBottom > canvBottom)
    {
        SetRelativeBottom(canvBottom);
    }
    else
    {
        //do nothing
    }
}

void DragButton::RestrictHorizon()
{
    //tmp
//    return;

    int canvLeft = area->x;
    int canvRight = area->x + area->w;

    int selfLeft = GetRelativeLeftTop().x;
    int selfRight = GetRelativeLeftTop().x + drawSize.x;

    if(selfLeft < canvLeft)
    {
        SetRelativeLeft(canvLeft);
    }
    else if(selfRight > canvRight)
    {
        SetRelativeRight(canvRight);
    }
    else
    {
        //do nothing
    }
}

void DragButton::SetXRatio(float xRatio)
{
    SetRelativeLeft((area->w - drawSize.x) * xRatio + area->x);
//    float tmp = ReportHorizonRatio();
}
void DragButton::SetYRatio(float yRatio)
{
    SetRelativeTop((area->h - drawSize.y) * yRatio + area->y);
//    float tmp = ReportVerticleRatio();
}

DBG_Status DragButton::OnButtonPressed()
{
    DBG_Status status = DBG_OK;

    status |= Button::OnButtonPressed();

//    followMouse = true;

    //set the size anchor
    SDL_Rect absDestRect = GetAbsDestRect();    //temp: 待优化
    sizeAnchor = SDL_Point{absDestRect.x, absDestRect.y};
    sizeAnchor.x = motherScene->inputHandler->GetMousePosition().x - sizeAnchor.x;
    sizeAnchor.y = motherScene->inputHandler->GetMousePosition().y - sizeAnchor.y;

    //temp: to remove Scene->mouseOccupiedGUIComp
    //occupy the mouse
//    motherScene->mouseOccupiedGUIComp = this;
    //it's not elegant, so you should try not to put a drag button in a canvas with other kinds of selectable gui

    return status;
}

DBG_Status DragButton::OnButtonReleased()
{
    DBG_Status status = DBG_OK;

    status |= Button::OnButtonReleased();

//    followMouse = false;

//    motherScene->mouseOccupiedGUIComp = NULL;

    return status;
}

DBG_Status DragButton::OnSelected()
{
    DBG_Status status = DBG_OK;

    status |= Button::OnSelected();

    return status;
}

DBG_Status DragButton::OnUnSelected()
{
    DBG_Status status = DBG_OK;

    status |= Button::OnUnSelected();

    //temp!: not smooth
    //try to use Scene's pressedButton(no implementation yet)
//    followMouse = false;
//    motherScene->mouseOccupiedGUIComp = NULL;

    return status;
}

int ScrollBar::scrollBarSize = 16;

ScrollBar::ScrollBar(Canvas* attachedCanvas, ScrollBarWay way)
    :GameComp(), way(way)
{
    color = GetColor(LightGray);    //default color
    rollBar = NULL;
    minusButton = NULL;
    addButton = NULL;
    rollBackground = NULL;
//    this->way = way;
    this->attachedCanvas = attachedCanvas;
    if(attachedCanvas == NULL)
    {
        ENG_LogError("attachedCanvas for ScrollBar can't be NULL!");
    }
    else
    {
        int canvasLeft, canvasTop, canvasRight, canvasBottom;
        attachedCanvas->GetRelativeLeftTop(&canvasLeft, &canvasTop);
        attachedCanvas->GetRelativeRightBottom(&canvasRight, &canvasBottom);
        Canvas* motherCanvas = dynamic_cast<Canvas*>(attachedCanvas->GetAttachedPlatform());
        SDL_Color backgroundColor = SDL_Color{color.r < 70 ? 0 : color.r - 70,
                                              color.g < 70 ? 0 : color.g - 70,
                                              color.b < 70 ? 0 : color.b - 70,
                                              color.a};
        SDL_Rect moveArea;
        //malloc
        switch(way)
        {
        case scrVerticle:

            moveArea = SDL_Rect{canvasRight + ScrollBar::scrollBarSize / 2, canvasTop + ScrollBar::scrollBarSize,
                                         0, canvasBottom - canvasTop - ScrollBar::scrollBarSize * 2};
            rollBar = new DragButton(canvasRight + ScrollBar::scrollBarSize / 2, canvasTop + ScrollBar::scrollBarSize,
                                     color, SDL_Point{ScrollBar::scrollBarSize, ScrollBar::scrollBarSize * 3},  //temp
                                     &moveArea, motherCanvas);
            minusButton = new Button(canvasRight + ScrollBar::scrollBarSize / 2, canvasTop + ScrollBar::scrollBarSize / 2,
                                     color, SDL_Point{ScrollBar::scrollBarSize, ScrollBar::scrollBarSize}, motherCanvas);
            addButton = new Button(canvasRight + ScrollBar::scrollBarSize / 2, canvasBottom - ScrollBar::scrollBarSize / 2,
                                   color, SDL_Point{ScrollBar::scrollBarSize, ScrollBar::scrollBarSize}, motherCanvas);
            rollBackground = new GUI(canvasRight + ScrollBar::scrollBarSize / 2, (canvasTop + canvasBottom) / 2,
                                     SDL_Point{ScrollBar::scrollBarSize, canvasBottom - canvasTop}, backgroundColor,
                                     0.5f, motherCanvas);

//            attachedCanvas->verticleScrollBar = this;

            //temp
            //ENG_LogInfo("Create a verticle scroll bar");

            break;

        case scrHorizon:
        default:

            moveArea = SDL_Rect{canvasLeft + ScrollBar::scrollBarSize, canvasBottom + ScrollBar::scrollBarSize / 2,
                                         canvasRight - canvasLeft - ScrollBar::scrollBarSize * 2, 0};
            rollBar = new DragButton(canvasLeft + ScrollBar::scrollBarSize, canvasBottom + ScrollBar::scrollBarSize / 2,
                                     color, SDL_Point{ScrollBar::scrollBarSize * 3, ScrollBar::scrollBarSize},    //temp
                                     &moveArea, motherCanvas);
            minusButton = new Button(canvasLeft + ScrollBar::scrollBarSize / 2, canvasBottom + ScrollBar::scrollBarSize / 2,
                                     color, SDL_Point{ScrollBar::scrollBarSize, ScrollBar::scrollBarSize}, motherCanvas);
            addButton = new Button(canvasRight - ScrollBar::scrollBarSize / 2, canvasBottom + ScrollBar::scrollBarSize / 2,
                                   color, SDL_Point{ScrollBar::scrollBarSize, ScrollBar::scrollBarSize}, motherCanvas);
            rollBackground = new GUI((canvasLeft + canvasRight) / 2, canvasBottom + ScrollBar::scrollBarSize / 2,
                                     SDL_Point{canvasRight - canvasLeft, ScrollBar::scrollBarSize}, backgroundColor,
                                     0.5f, motherCanvas);

//            attachedCanvas->horizonScrollBar = this;

            //temp
            //ENG_LogInfo("Create a horizon scroll bar");

            break;
        }
        if(rollBar && minusButton && addButton && rollBackground)
        {
            rollBar->ResetFollowingCanvas(attachedCanvas);
            minusButton->ResetFollowingCanvas(attachedCanvas);
            addButton->ResetFollowingCanvas(attachedCanvas);
            rollBackground->ResetFollowingCanvas(attachedCanvas);
        }
        else
        {
            ENG_LogError("Cannot malloc sub-components for ScrollBar.");
        }
    }
}

ScrollBar::~ScrollBar()
{
    switch(way)
    {
    case scrVerticle:
//        attachedCanvas->verticleScrollBar = NULL;     //handled by scene event handler
        //temp
        //ENG_LogInfo("Delete a verticle scroll bar");
        break;

    case scrHorizon:
    default:
//        attachedCanvas->horizonScrollBar = NULL;
        //temp
        //ENG_LogInfo("Delete a verticle scroll bar");
        break;
    }

    if(rollBar)
        delete rollBar;

    if(minusButton)
        delete minusButton;

    if(addButton)
        delete addButton;

    if(rollBackground)
        delete rollBackground;
}

DBG_Status ScrollBar::InitInScene(Scene* scene)
{
    DBG_Status status = DBG_OK;

    status |= GameComp::InitInScene(scene);
    if(status & DBG_REP_OPR)
        return status;

    *scene << rollBackground;
    *scene << rollBar;
    *scene << minusButton;
    *scene << addButton;

    return status;
}

DBG_Status ScrollBar::DumpOutOfScene()
{
    DBG_Status status = DBG_OK;

    *motherScene >> rollBackground;
    *motherScene >> rollBar;
    *motherScene >> minusButton;
    *motherScene >> addButton;

    status |= GameComp::DumpOutOfScene();
    return status;
}

DBG_Status ScrollBar::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    status |= GameComp::Update(deltTick);

    int canvasLeft, canvasTop, canvasRight, canvasBottom;
    attachedCanvas->GetRelativeLeftTop(&canvasLeft, &canvasTop);
    attachedCanvas->GetRelativeRightBottom(&canvasRight, &canvasBottom);
    switch(way)
    {
    case scrVerticle:

        attachedCanvas->SetScrollVerticle(rollBar->ReportVerticleRatio());
        //follow the attached canvas (temp: necessary ???)
#ifdef SCROLLBAR_FOLLOW_CANVAS
        rollBar->SetAreaLeftTop(canvasRight + ScrollBar::scrollBarSize / 2,
                                canvasTop + ScrollBar::scrollBarSize);
        rollBackground->SetRelativeLeftTop(canvasRight, canvasTop);
        minusButton->SetRelativeLeftTop(canvasRight, canvasTop);
        addButton->SetRelativeLeftTop(canvasRight, canvasBottom - ScrollBar::scrollBarSize);
#endif // SCROLLBAR_FOLLOW_CANVAS

        break;

    case scrHorizon:
    default:

        attachedCanvas->SetScrollHorizon(rollBar->ReportHorizonRatio());
        //follow the attached canvas (temp: necessary ???)
#ifdef SCROLLBAR_FOLLOW_CANVAS
        rollBar->SetAreaLeftTop(canvasLeft + ScrollBar::scrollBarSize,
                                canvasBottom + ScrollBar::scrollBarSize / 2);
        rollBackground->SetRelativeLeftTop(canvasLeft, canvasBottom);
        minusButton->SetRelativeLeftTop(canvasLeft, canvasBottom);
        addButton->SetRelativeLeftTop(canvasRight - ScrollBar::scrollBarSize, canvasBottom);
#endif // SCROLLBAR_FOLLOW_CANVAS

        break;
    }

    return status;
}

}
