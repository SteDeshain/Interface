//gui.h
//created by SteDeshain
//created on 2016/08/14
//latest updated on 2016/08/14
//GUI class inherited from DrawableComp, as a abstract class, to create other UI classes

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

class GUI: public DrawableComp
{
    friend class Scene;
    friend class GUIOperateHandler;

public:
    GUI(int x, int y, int textureNum, const char* imgFile = NULL, Canvas* canvas = NULL);
    GUI(int x, int y, int drawIndex, Canvas* canvas = NULL);    //for textureNum == 1
    GUI(int textureNum, const char* imgFile = NULL, Canvas* canvas = NULL);
    ~GUI();

    bool IsSelectable();
    bool IsSelected();

    virtual void SetExtraTrans(float extraTrans);

protected:
//    GUILayer layer; //smaller number is drawn first
    int drawIndex;  //the order to be drawn in a canvas, smaller number is drawn first

    SDL_Point drawSize;
    SDL_Point entireSize;
    SDL_Point sourceStartPoint;

    float extraTrans;       //extra transparency apply to RenderCopy, inner use, from 0.0 to 1.0
    Uint8 textureAlpha;

    bool selectable;
    bool selected;

    virtual DBG_Status HandleEvent(SDL_Event event);
    virtual DBG_Status InitInScene(Scene *scene);
    virtual DBG_Status Update(Uint32 deltTick);
    virtual SDL_Rect GetAbsDestRect(int* x = NULL, int* y = NULL, int* w = NULL, int* h = NULL);

    virtual SDL_Rect CutSrcRect(SDL_Rect srcRect, SDL_Point destSize);  //裁剪sourceRect以适应destRect

//    virtual DBG_Status LoadTexture();

private:

};

enum SlideWay
{
    SlideUpDown,
    SlideDownUp,
    SlideLeftRight,
    SlideRightLeft,
};

class Canvas: public GUI
{
    friend class GUI;

public:
    Canvas(SDL_Color color, float transparency, SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas);
    Canvas(const char* imgFile, float transparency, SDL_Rect viewRect, SDL_Point canvasSize, Canvas* motherCanvas);

    ~Canvas();

//    void CalculateDrawRect();   //calculate the final absolute drawRect to be used in Draw finction

    //temp
    void SetScrollOffset(int dx, int dy);

    virtual void SetExtraTrans(float extraTrans);

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

    //show process animation
//    SlideWay slideWay;      //slide direction when shown, the opposite direction when disappeared
//    Uint32 slideTicks;      //slide time
    struct SlideInfo
    {
        SlideWay slideWay;
        Uint32 slideTicks;
    }* slideInfo;

    virtual DBG_Status InitInScene(Scene* scene);

    virtual SDL_Rect CutSrcRect(SDL_Rect srcRect, SDL_Point destSize);

};

class Button: public GUI
{

public:
    Button(int x, int y, SDL_Color color, SDL_Point buttonSize, Canvas* motherCanvas);

protected:
    DBG_Status InitInScene(Scene* scene);
    DBG_Status HandleEvent(SDL_Event event);

    DBG_Status OnButtonPressed();
    DBG_Status OnButtonReleased();

    SDL_Color color;

};

}

#endif // __GUI_H__
