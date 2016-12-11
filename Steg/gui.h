#ifndef __GUI_H__
#define __GUI_H__

#include "SDL2/SDL.h"
#include "drawable_comp.h"

namespace steg
{

enum GUILayer
{
//    UnderScene = 0,     //drawn into the scene together with SolidObjs, like on the ground there is a text. It's awesome
                        //actually added into SceneComps(SolidObjs), so I abonded it, I will implate another class for this
    AboveScene = 0,     //like health bar or the item bar on screen
    Menu = 1,
    //...
    DebugUI = 1000,
};

class Canvas;

//class GUI: virtual public DrawableComp
class GUI: public DrawableComp
{
    friend class Scene;
    friend class GUIOperateHandler;

public:
    GUI(int x, int y, int textureNum, const char* imgFile, Canvas* canvas = NULL);
    GUI(int x, int y, int drawIndex, Canvas* canvas = NULL);    //for textureNum == 1, used by Lable
    GUI(int textureNum, const char* imgFile, Canvas* canvas = NULL);
    //full version
    //picture mode
    GUI(const char* name, int x, int y, int drawIndex,
        int textureNum, const char* imgFile, steg::Canvas* canvas);
    //color mode
    GUI(const char* name, int x, int y, int drawIndex,
        SDL_Point picSize, SDL_Color color, float transparency, steg::Canvas* canvas);

    //get color picture
    GUI(int x, int y, SDL_Point picSize, SDL_Color color, float transparency, Canvas* canvas = NULL);   //make a copy of color

    ~GUI();

    bool IsSelectable();
    bool IsSelected();

    virtual void SetExtraTrans(float extraTrans);
    virtual float GetExtraTrans();

    SDL_Point GetDrawSize();
    SDL_Point GetEntireSize();

    void ResetFollowingCanvas(Canvas* newFollowingCanvas);

protected:
//    GUILayer layer; //smaller number is drawn first
    int drawIndex;  //the order to be drawn in a canvas, smaller number is drawn first

    SDL_Color* color = NULL;    //是否为NULL,以和无texture的gui区别开来
    SDL_Point colorTextureSize = SDL_Point{0, 0};
    float colorTextureTransparency = 1.0f;

    SDL_Point drawSize;
    SDL_Point entireSize;
    SDL_Point sourceStartPoint;

    float extraTrans;       //extra transparency apply to RenderCopy, inner use, from 0.0 to 1.0
    Uint8 textureAlpha;

    bool selectable;
    bool selected;

    virtual DBG_Status HandleEvent(SDL_Event event);
    virtual DBG_Status InitInScene(Scene *scene);
    virtual DBG_Status DumpOutOfScene();
    virtual DBG_Status Update(Uint32 deltTick);
    virtual SDL_Rect GetAbsDestRect(int* x = NULL, int* y = NULL, int* w = NULL, int* h = NULL);

    virtual SDL_Rect CutSrcRect(SDL_Rect srcRect, SDL_Point destSize);  //裁剪sourceRect以适应destRect

//    virtual DBG_Status LoadTexture();

    virtual DBG_Status OnSelected();
    virtual DBG_Status OnUnSelected();

private:
    bool lastVisible = false;

    //this followingCanvas is used in Update() to set extra transparency
    //by default, the followingCanvas is the attachedPlatform
    //but for scroll bar's components, their followingCanvas should be the canvas controled by the scroll bar
    //rather than their attachedPlatform
    DrawableComp* followingCanvas = NULL;

};

enum SlideWay
{
    SlideUp,
    SlideDown,
    SlideLeft,
    SlideRight,
};

struct CanvasState
{
    DBG_Status (* Update)(Uint32 deltTick, Canvas* canvas, CanvasState* self);
    DBG_Status (* HandleEvent)(SDL_Event event, Canvas* canvas, CanvasState* self);

    float totalMove;    //有时movePerTick太小，直接加到position中不会有影响
//    SDL_Point destPosition; //final position when idle, no necessary
//    float movePerTick;        //
//    float deltTransPerTick;   //read from slideInfo
};

class ScrollBar;

//class Canvas: virtual public GUI
class Canvas: public GUI
{
    friend class GUI;
    friend class ScrollBar;
    friend class Scene;
//    friend DBG_Status IdleHandleEvent(SDL_Event event, Canvas* canvas, CanvasState* self);
//    friend DBG_Status ShowUpdate(Uint32 deltTick, Canvas* canvas, CanvasState* self);

public:
    //picture mode
    Canvas(const char* name, const char* imgFile, float transparency,
           SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible);
    //color mode
    Canvas(const char* name, SDL_Color color, float transparency,
           SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas, bool startVisible);
    Canvas(SDL_Color color, float transparency, SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas);
    Canvas(const char* imgFile, float transparency, SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas);

    ~Canvas();

//    void CalculateDrawRect();   //calculate the final absolute drawRect to be used in Draw finction

    //temp
    void SetScrollOffset(int dx, int dy);
    void SetScrollHorizon(float xRatio);
    void SetScrollVerticle(float yRatio);
    SDL_Point GetScrollOffset(int* x = NULL, int* y = NULL);

    virtual void SetExtraTrans(float extraTrans);
    virtual float GetExtraTrans();
    void AddExtraTrans(float deltaTrans);

    struct SlideInfo
    {
        SlideWay showWay;
        SlideWay hideWay;
        Uint32 slideTicks;  //slide time
        unsigned int slideDistance;

        float deltTransPerTick;
        float movePerTick;
        SDL_Point destPos;  //final position when idel
    };
    void SetSlideInfo(SlideWay showWay, SlideWay hideWay, Uint32 slideTicks = 200, unsigned int slideDistance = 10);
    void SetSlideInfo();
    SlideInfo* GetSlideInfo();

    enum CanvasStateEnum
    {
        StateEnumShow,
        StateEnumIdle,
        StateEnumHide,
    };
    void SetCanvasState(CanvasStateEnum state);

//	virtual bool IsVisible();
	virtual void SetVisible(bool visible);

	CanvasState* GetState(CanvasStateEnum stateEnum);

	bool GetOriginVisible();

	ScrollBar* GetHorizonScrollBar();
	ScrollBar* GetVerticleScrollBar();

protected:
    //Canvas* motherCanvas;     //actually it's the attached platform
//    SDL_Rect viewRect;      //view rect to be drawn relative to mother canvas' left-top
                            //actually it's the drawRect
    //SDL_Point canvasSize;   //whole canvas' area to draw guis and other canvases
                            //actually, it's the entireSize
                            //the canvasRect cannot be smaller than viewRect's size
                            //usually they have the same size
                            //the canvasRect's x and y are relative to viewRect's left-top
                            //there will be one or two scroll bars when the canvasRect is bigger than the viewRect
//    SDL_Rect drawRect;    //the final drawRect
//    SDL_Point srcRectRelaPos;   //the relative position to drawRect's left-top
                                //use it and drawRect's w and h to calculate the final sourceRect
    SDL_Point scrollOffset;

    SDL_Color color;        //if canvas has no texture, it will be drawn as a single color
    float transparency;     //from 0.0(invisible) to 1.0(no transparency), origin transparency

    float originExtraTrans; //restore origin extra trans value
    bool originVisible;     //restore origin visible value

    bool startVisible = false;  //visible when insert into a scene

    //show process animation
    SlideInfo* slideInfo;

    CanvasState canvasShow;
    CanvasState canvasIdle;
    CanvasState canvasHide;
    CanvasState* currentState;

    ScrollBar* horizonScrollBar = NULL;
    ScrollBar* verticleScrollBar = NULL;

    virtual DBG_Status InitInScene(Scene* scene);   //check whether need to add or remove scroll bar
    virtual DBG_Status DumpOutOfScene();
    virtual DBG_Status Update(Uint32 deltTick);
    virtual DBG_Status HandleEvent(SDL_Event event);

    DBG_Status ResetTexture(const char* newImg = NULL);             //check whether need to add or remove or reset scroll bar
    DBG_Status ResetTexture(SDL_Point newSize = SDL_Point{0, 0});   //check whether need to add or remove or reset scroll bar

    virtual SDL_Rect CutSrcRect(SDL_Rect srcRect, SDL_Point destSize);

private:
    int horzScrollBarPos = 0; //retsored before animation state
    int vertScrollBarPos = 0;

};

//class Button: virtual public GUI
class Button: public GUI
{

public:
    Button(int x, int y, SDL_Color color, SDL_Point buttonSize, Canvas* motherCanvas);
    Button(int x, int y, const char* imgFile, Canvas* motherCanvas);

    ~Button();

protected:
    virtual DBG_Status InitInScene(Scene* scene);
    virtual DBG_Status DumpOutOfScene();
    virtual DBG_Status HandleEvent(SDL_Event event);

    virtual DBG_Status OnButtonPressed();
    virtual DBG_Status OnButtonReleased();
    virtual DBG_Status OnButtonDumped();

    virtual DBG_Status OnSelected();
    virtual DBG_Status OnUnSelected();

    SDL_Color color;
    bool buttonDown = false;

};

//class DragButton: virtual public Button
class DragButton: public Button
{
    friend DBG_Status ShowUpdate(Uint32 deltTick, Canvas* canvas, CanvasState* self);

public:
    DragButton(int x, int y, SDL_Color color, SDL_Point buttonSize, SDL_Rect* area, Canvas* motherCanvas);
    DragButton(int x, int y, const char* imgFile, SDL_Rect* area, Canvas* motherCanvas);

    ~DragButton();

    float ReportHorizonRatio();
    float ReportVerticleRatio();

    void SetAreaPos(int x, int y);
    void SetAreaLeftTop(int left, int top);

protected:
    SDL_Rect* area;

    virtual DBG_Status InitInScene(Scene* scene);
    virtual DBG_Status DumpOutOfScene();
    virtual DBG_Status Update(Uint32 deltTick);

    virtual DBG_Status OnButtonPressed();
    virtual DBG_Status OnButtonReleased();

    virtual DBG_Status OnSelected();
    virtual DBG_Status OnUnSelected();

//    bool followMouse = false;

    SDL_Point sizeAnchor;

private:
    void RestrictPosition();
    void RestrictVertical();
    void RestrictHorizon();

    void SetXRatio(float xRatio);
    void SetYRatio(float yRatio);

};

enum ScrollBarWay: int
{
    scrHorizon = 0,     //NULL for horizon
    scrVerticle = 1,
};

//class ScrollBar: virtual public GameComp
class ScrollBar: public GameComp
{

//    friend DBG_Status IdleHandleEvent(SDL_Event event, Canvas* canvas, CanvasState* self);
//    friend DBG_Status ShowUpdate(Uint32 deltTick, Canvas* canvas, CanvasState* self);

public:
    static int scrollBarSize;

public:
    ScrollBar(Canvas* attachedCanvas, ScrollBarWay way);    //default look

    ~ScrollBar();

protected:
    SDL_Color color;
    DragButton* rollBar;
    Button* minusButton;
    Button* addButton;
    GUI* rollBackground;
    const ScrollBarWay way;
    Canvas* attachedCanvas;

    virtual DBG_Status InitInScene(Scene* scene);
    virtual DBG_Status DumpOutOfScene();
    virtual DBG_Status Update(Uint32 deltTick);     //where control the canvas show and scroll bar update

};

}

#endif // __GUI_H__
