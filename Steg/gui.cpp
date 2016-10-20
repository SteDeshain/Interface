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
}

GUI::GUI(int x, int y, int drawIndex, Canvas* canvas)
    :DrawableComp(x, y, 1, NULL, canvas), drawIndex(drawIndex)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
}

GUI::GUI(int textureNum, const char* imgFile, Canvas* canvas)
    :DrawableComp(textureNum, imgFile, canvas)
{
    selectable = false;
    selected = false;
    sourceStartPoint = SDL_Point{0, 0};
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

    return status;
}

//DBG_Status GUI::LoadTexture()
//{
//    entireSize = SDL_Point{sourceRect.w, sourceRect.h};
//}

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

Canvas::Canvas(SDL_Color color, float transparency, SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas)
    :GUI(1, NULL, motherCanvas),
     color(color), transparency(transparency)
{
    entireSize = canvasSize;
    drawSize = SDL_Point{viewRect.w, viewRect.h};
    SetRelativeDrawRect(viewRect);
    scrollOffset = SDL_Point{0, 0};
}

Canvas::Canvas(const char* imgFile, float transparency, SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas)
    :GUI(1, imgFile, motherCanvas),
     color(GetColor(Black)), transparency(transparency)
{
    entireSize = canvasSize;
    drawSize = SDL_Point{viewRect.w, viewRect.h};
    SetRelativeDrawRect(viewRect);
    scrollOffset = SDL_Point{0, 0};
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
        SDL_Rect rect2 = SDL_Rect{entireSize.x, 0, entireSize.x, 2};
        SDL_Rect rect3 = SDL_Rect{entireSize.x, 0, 2, entireSize.y};
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
