//the scene only containing gui components, no physics in it
//like srart menu, or a scene showing studio and other info...

#ifndef __GUI_SCENE_H__
#define __GUI_SCENE_H__

#include "interface_scene.h"
#include "stel.h"
#include "game_comp.h"

namespace interface
{

class GuiScene: public InterfaceScene
{

public:
    GuiScene(const char* name, SDL_Renderer* render);
    ~GuiScene();

    virtual DBG_Status InitScene();
	virtual DBG_Status Update(Uint32 deltTick);
    virtual DBG_Status UpdatePhysics(Uint32 reminingTick);

protected:

};

}

#endif // __GUI_SCENE_H__
