//the scene only containing gui components, no physics in it
//like srart menu, or a scene showing studio and other info...

#ifndef __GUI_SCENE_H__
#define __GUI_SCENE_H__

#include "interface_scene.h"
#include "stel.h"

class GuiScene: public InterfaceScene
{

public:
    GuiScene(SDL_Renderer* render, const char* name);
    ~GuiScene();

    virtual DBG_Status InitScene();
	virtual DBG_Status Update(Uint32 deltTick);
    virtual DBG_Status UpdatePhysics(Uint32 reminingTick);

};

#endif // __GUI_SCENE_H__
