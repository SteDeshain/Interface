//label.h
//created by SteDeshain
//created on 2016/08/14
//latest updated on 2016/08/14
//label class to draw text
//this Label class has one texture

#ifndef __LABEL_H__
#define __LABEL_H__

#include <string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "gui.h"

namespace steg
{
enum LabelAnchor
{
    LeftTop,
    Center,
    RightTop,
};

//class Label: virtual public GUI
class Label: public GUI
{

public:
    Label(const char* name, int x, int y, steg::LabelAnchor anchor,
          const char* text, TTF_Font* font, SDL_Color color, int drawIndex, steg::Canvas* canvas);
    //the x and y is the anchor point point
    Label(int x, int y, LabelAnchor anchor, const char* text, TTF_Font* font, SDL_Color color, int drawIndex, Canvas* canvas = NULL);
    ~Label();

    virtual DBG_Status ReloadTexture(const char* newText = NULL);

protected:
    const char* text;       //there may be a need to point to another text
    TTF_Font* font;
    SDL_Color color;
    LabelAnchor anchor;     //defualt anchor is at left
    SDL_Point anchorPoint;

	virtual DBG_Status InitInScene(Scene *scene);	//called after inserted into a Scene
	virtual DBG_Status DumpOutOfScene();			//called after thrown out by a Scene

    virtual DBG_Status LoadTexture();

};

}

#endif  // __LABEL_H__
