#include "scene1.h"
#include "SDL2/SDL_ttf.h"
#include "stdio.h"
#include "game.h"
#include "color.h"
#include "event_handler.h"

namespace gameTest
{

Scene1::Scene1(int mapWidthNum, int mapHeightNum, SDL_Renderer* render)
    :steg::Scene(mapWidthNum, mapHeightNum, render)
{

}

Scene1::~Scene1()
{

}

DBG_Status Scene1::InitScene()
{
    using namespace steg;

    DBG_Status status = DBG_OK;

    menuFont = TTF_OpenFont("LanTing.ttf", 24);

    rects = new SDL_Rect[mapHeightNum * mapWidthNum];
    sRects = new SDL_Rect[mapHeightNum * mapWidthNum];
	for(int i = 0; i < mapHeightNum; i++)      //共10行
	{
        for(int j = 0; j < mapWidthNum; j++)   //每行有15列
        {
            rects[i * mapWidthNum + j] = SDL_Rect{j * Game::tileWidth, i * Game::tileWidth, Game::tileWidth, Game::tileWidth};
            sRects[i * mapWidthNum + j] = SDL_Rect{5 * 64, 0, 64, 64};
        }
	}
	ground = new MultiDrawComp(mapWidthNum * Game::tileWidth / 2,
                               mapHeightNum * Game::tileWidth / 2,
                               mapWidthNum * mapHeightNum,
                               rects, sRects, 1, "tiles.png");

    int platformWidth = 3;
    int platformHeight = 3;
    rects2 = new SDL_Rect[platformWidth * platformHeight];
    sRects2 = new SDL_Rect[platformWidth * platformHeight];
    for(int i = 0; i < platformHeight; i++)
    {
        for(int j = 0; j < platformWidth; j++)
        {
            rects2[i * platformWidth + j] = SDL_Rect{j * Game::tileWidth, i * Game::tileWidth, Game::tileWidth, Game::tileWidth};
//            sRects2[i * platformHeight + j] = SDL_Rect{4 * 64, 0, 64, 64};
        }
    }
    sRects2[0] = SDL_Rect{5 * 64, 2 * 64, 64, 64};
    sRects2[1] = SDL_Rect{6 * 64, 2 * 64, 64, 64};
    sRects2[2] = SDL_Rect{7 * 64, 2 * 64, 64, 64};
    sRects2[3] = SDL_Rect{5 * 64, 3 * 64, 64, 64};
    sRects2[4] = SDL_Rect{6 * 64, 3 * 64, 64, 64};
    sRects2[5] = SDL_Rect{7 * 64, 3 * 64, 64, 64};
    sRects2[6] = SDL_Rect{2 * 64, 3 * 64, 64, 64};
    sRects2[7] = SDL_Rect{3 * 64, 3 * 64, 64, 64};
    sRects2[8] = SDL_Rect{4 * 64, 3 * 64, 64, 64};
    SolidObjInfo info;
    info.InitAsNormal(30, 128);
    info.SetPosition(150, 200, 0);
    info.gravitied = false;
    info.mass = REAL_MAX;
    info.altitude = 20;
    info.elasticity = 0.0f;
    info.collideType = CollideC | CollideA;
    platform = new MultiDrawSolidObj(info, platformHeight * platformWidth, rects2, sRects2, "tiles.png", 1);

    info.collideType = CollideA;
//    EnableAllCollidePairs();
    SetCollidePair('a', 'a');
    SetCollidePair('a', 'b');
    SetCollidePair('b', 'b');
    SetCollidePair('c', 'c');

    mountain = new DrawableComp("tiles.png", ground);
	camPos = new Label(0, 15, LeftTop, "cam's pos", debugFont, GetColor(Black), 1);
//	SolidObjInfo info = {.x = 200, .y = 200,
//                         .altitude = 100,
//                         .length = 50, .width = 20, .height = 100,
//                         .gravitied = true,
//                         .velocity = stef::Vector3::ZERO,
//                         .acceleration = stef::Vector3::ZERO,
//                         .damping = 0.9f, .mass = 20.0f, .elasticity = 1.0f,
//                         .collideType = CollideA, .detectType = CollideAll, .collidable = true};
    info.InitAsNormal(20.0f, 20);
    info.SetPosition(150, 200, 200);
//    info.triggered = true;
//    info.gravitied = false;
//    info.mass = REAL_MAX;
//    info.elasticity = 0;

//    character = new SolidObj(info, "character.png");
    character = new Character(info, "character.png", 1);

    info.SetPosition(250, 300, 600);
//    info.SetPosition(150, 200, 600);
    info.gravitied = true;
    info.mass = 20.0f;
    info.triggered = false;
//    info.elasticity = 1.0f;
    char2 = new SolidObj(info, "character.png");

    info.collideType = CollideB;
    info.SetPosition(400, 100, 300);
    char3 = new SolidObj(info, "character.png");

    info.SetPosition(550, 300, 300);
    info.width = 30;
//    info.collidable = false;
#if 1
    info.mass = 100.0f;
#else
    info.mass = REAL_MAX;
    info.gravitied = false;
    info.altitude = 0;
#endif // 0
    char4 = new SolidObj(info, "character.png");
//    char4->ToggleCollidability();


    mapInfo = new Label(Game::windowWidth, 0,
                        RightTop, "map info", debugFont, GetColor(Black), 2);
	compCount  = new Label(Game::windowWidth, 15,
                           RightTop, "GameComp num:", debugFont, GetColor(Black), 2);
	drawableCount = new Label(Game::windowWidth, 30,
                              RightTop, "DrawableComp num: ", debugFont, GetColor(Black), 2);
	GUICount = new Label(Game::windowWidth, 45,
                         RightTop, "GUIComp num: ", debugFont, GetColor(Black), 2);
	SolidCount = new Label(Game::windowWidth, 60,
                           RightTop, "SolidObj num: ", debugFont, GetColor(Black), 2);
    posValue = new Label(Game::windowWidth, 75,
                         RightTop, "Pos: ", debugFont, GetColor(Black), 2);
    charsFrontObjsNum = new Label(Game::windowWidth, 90,
                                  RightTop, "frontObjs num: ", debugFont, GetColor(Black), 2);
    resortCount = new Label(Game::windowWidth, 105,
                            RightTop, "resorted: ", debugFont, GetColor(Black), 2);

    SDL_Rect viewRect = SDL_Rect{20, 20, 360, 400};
    SDL_Point canvasSize = SDL_Point{400, 500};
    motherCanv = new Canvas(GetColor(LightGray), 0.5f, viewRect, canvasSize, NULL);
    motherCanv->SetSlideInfo();
    motherCanv->HideComp();
    viewRect = SDL_Rect{20, 20, 200, 200};
//    viewRect = SDL_Rect{-80, 20, 200, 200};
    canvasSize = SDL_Point{300, 300};
//    Canvas* canv = new Canvas(GetColor(Cyan), 0.5f, viewRect, canvasSize, motherCanv);
    canv = new Canvas("tiles.png", 0.5f, viewRect, canvasSize, motherCanv);
    canv->SetSlideInfo();
    Label* temp = new Label(10, 10, LeftTop, "hello", menuFont, GetColor(White), 2, canv);
    Label* temp2 = new Label(240, 20, LeftTop, "你好", menuFont, GetColor(White), 2, motherCanv);

    childCanv = new Canvas(GetColor(Cyan), 0.7f, SDL_Rect{10, 90, 160, 150}, SDL_Point{160, 150}, canv);
    childCanv->SetSlideInfo();
    Label* temp3 = new Label(20, 20, LeftTop, "画布显示测试", menuFont, GetColor(White), 2, childCanv);
    Button* but3 = new Button(60, 70, GetColor(LightGray), SDL_Point{60, 30}, childCanv);

//    Button* but = new Button(25, 60, GetColor(LightGray), SDL_Point{30, 30}, canv);
    Button* but = new Button(25, 60, "button.png", canv);
    SDL_Rect sizeArea = SDL_Rect{30, 25, 200, 0};
//    DragButton* but2 = new DragButton(100, 60, GetColor(LightGray), SDL_Point{60, 30}, &sizeArea, canv);
    DragButton* but2 = new DragButton(100, 60, GetColor(LightGray), SDL_Point{60, 30}, NULL, canv);
//    DragButton* but2 = new DragButton(100, 60, GetColor(LightGray), SDL_Point{60, 30}, NULL, NULL);

    GUI* pic = new GUI(50, 300, 1, "character.png", motherCanv);

	*this << ground << mountain << camPos << drawableCount << GUICount << SolidCount << posValue << charsFrontObjsNum
          << resortCount
          << platform
          << compCount << mapInfo << character << char2 << char3 << char4;
    *this << motherCanv << canv << temp << temp2;
    *this << but << pic << childCanv << temp3 << but3;
    *this << but2;

//    canv->SetExtraTrans(0.8f);
//    motherCanv->SetExtraTrans(0.8f);

	mountain->SetDrawSize(64, 64);
	mountain->SetSourceRect(SDL_Rect{64, 64, 64, 64});
	mountain->SetRelativePos(110, 100);

	resortCt = 0;

	sprintf(buffer, "camera's position: (%d, %d)", camera->GetLeftTop().x, camera->GetLeftTop().y);
	camPos->ReloadTexture(buffer);
	sprintf(buffer, "map size: %d x %d", mapWidthNum, mapHeightNum);
	mapInfo->ReloadTexture(buffer);
	sprintf(buffer, "GameComp num: %d", comps.size());
	compCount->ReloadTexture(buffer);
	sprintf(buffer, "DrawableComp num: %d", drawableComps.size());
	drawableCount->ReloadTexture(buffer);
	sprintf(buffer, "GUIComp num: %d", GUIComps.size());
	GUICount->ReloadTexture(buffer);
	sprintf(buffer, "SolidObj num: %d", solidObjs.size());
	SolidCount->ReloadTexture(buffer);
	sprintf(buffer, "Pos: %f, %f, %f", character->GetBox().GetPosition().x,
                                       character->GetBox().GetPosition().y,
                                       character->GetBox().GetPosition().z);
	posValue->ReloadTexture(buffer);
	sprintf(buffer, "frontObjs num: %d, %d, %d, %d", character->frontObjs.size(),
                                                     char2->frontObjs.size(),
                                                     char3->frontObjs.size(),
                                                     char4->frontObjs.size());
	charsFrontObjsNum->ReloadTexture(buffer);
	sprintf(buffer, "resorted: %d", resortCt);
	resortCount->ReloadTexture(buffer);

	return status;
}

DBG_Status Scene1::HandleInput()
{
    DBG_Status status = DBG_OK;

    status |= Scene::HandleInput();

    int speed = 150;
    if(inputHandler->KeyPressed(SDL_SCANCODE_I))
        character->GetBox().SetVelocity(character->GetBox().GetVelocity().x, ToMeter(-speed), character->GetBox().GetVelocity().z);
//    else if(inputHandler->KeyReleased(SDL_SCANCODE_I))
//        character->GetBox().SetVelocity(character->GetBox().GetVelocity().x, 0, character->GetBox().GetVelocity().z);

    else if(inputHandler->KeyPressed(SDL_SCANCODE_K))
        character->GetBox().SetVelocity(character->GetBox().GetVelocity().x, ToMeter(speed), character->GetBox().GetVelocity().z);
//    else if(inputHandler->KeyReleased(SDL_SCANCODE_K))
//        character->GetBox().SetVelocity(character->GetBox().GetVelocity().x, 0, character->GetBox().GetVelocity().z);


    if(inputHandler->KeyPressed(SDL_SCANCODE_J))
        character->GetBox().SetVelocity(ToMeter(-speed), character->GetBox().GetVelocity().y, character->GetBox().GetVelocity().z);
//    else if(inputHandler->KeyReleased(SDL_SCANCODE_J))
//        character->GetBox().SetVelocity(0, character->GetBox().GetVelocity().y, character->GetBox().GetVelocity().z);

    else if(inputHandler->KeyPressed(SDL_SCANCODE_L))
        character->GetBox().SetVelocity(ToMeter(speed), character->GetBox().GetVelocity().y, character->GetBox().GetVelocity().z);
//    else if(inputHandler->KeyReleased(SDL_SCANCODE_L))
//        character->GetBox().SetVelocity(0, character->GetBox().GetVelocity().y, character->GetBox().GetVelocity().z);

    int camSpeed = 5;
    if(inputHandler->KeyDown(SDL_SCANCODE_W))
        camera->SetMove(0, -1 * camSpeed);
    else if(inputHandler->KeyDown(SDL_SCANCODE_S))
        camera->SetMove(0, camSpeed);
    if(inputHandler->KeyDown(SDL_SCANCODE_A))
        camera->SetMove(-1 * camSpeed, 0);
    else if(inputHandler->KeyDown(SDL_SCANCODE_D))
        camera->SetMove(camSpeed, 0);

    int scrollSpeed = 20;
    if(inputHandler->KeyPressed(SDL_SCANCODE_UP) && inputHandler->KeyUp(SDL_SCANCODE_LSHIFT))
        motherCanv->SetScrollOffset(0, scrollSpeed);
    if(inputHandler->KeyPressed(SDL_SCANCODE_DOWN) && inputHandler->KeyUp(SDL_SCANCODE_LSHIFT))
        motherCanv->SetScrollOffset(0, -scrollSpeed);
    if(inputHandler->KeyPressed(SDL_SCANCODE_LEFT) && inputHandler->KeyUp(SDL_SCANCODE_LSHIFT))
        motherCanv->SetScrollOffset(scrollSpeed, 0);
    if(inputHandler->KeyPressed(SDL_SCANCODE_RIGHT) && inputHandler->KeyUp(SDL_SCANCODE_LSHIFT))
        motherCanv->SetScrollOffset(-scrollSpeed, 0);

    if(inputHandler->KeyPressed(SDL_SCANCODE_UP) && inputHandler->KeyDown(SDL_SCANCODE_LSHIFT))
        canv->SetScrollOffset(0, scrollSpeed);
    if(inputHandler->KeyPressed(SDL_SCANCODE_DOWN) && inputHandler->KeyDown(SDL_SCANCODE_LSHIFT))
        canv->SetScrollOffset(0, -scrollSpeed);
    if(inputHandler->KeyPressed(SDL_SCANCODE_LEFT) && inputHandler->KeyDown(SDL_SCANCODE_LSHIFT))
        canv->SetScrollOffset(scrollSpeed, 0);
    if(inputHandler->KeyPressed(SDL_SCANCODE_RIGHT) && inputHandler->KeyDown(SDL_SCANCODE_LSHIFT))
        canv->SetScrollOffset(-scrollSpeed, 0);

    if(inputHandler->KeyPressed(SDL_SCANCODE_Z) && inputHandler->KeyUp(SDL_SCANCODE_LSHIFT))
        PushHideCanvasEvent(motherCanv);
    if(inputHandler->KeyPressed(SDL_SCANCODE_X) && inputHandler->KeyUp(SDL_SCANCODE_LSHIFT))
        PushShowCanvasEvent(motherCanv);

    if(inputHandler->KeyPressed(SDL_SCANCODE_Z) && inputHandler->KeyDown(SDL_SCANCODE_LSHIFT))
        PushHideCanvasEvent(canv);
    if(inputHandler->KeyPressed(SDL_SCANCODE_X) && inputHandler->KeyDown(SDL_SCANCODE_LSHIFT))
        PushShowCanvasEvent(canv);

    if(inputHandler->KeyPressed(SDL_SCANCODE_C))
        PushHideCanvasEvent(childCanv);
    if(inputHandler->KeyPressed(SDL_SCANCODE_V))
        PushShowCanvasEvent(childCanv);

    return status;
}

DBG_Status Scene1::Update(Uint32 deltTick)
{
    Scene::Update(deltTick);

    sprintf(buffer, "camera's position: (%d, %d)", camera->GetLeftTop().x, camera->GetLeftTop().y);
    camPos->ReloadTexture(buffer);
	sprintf(buffer, "GameComp num: %d", comps.size());
	compCount->ReloadTexture(buffer);
	sprintf(buffer, "DrawableComp num: %d", drawableComps.size());
	drawableCount->ReloadTexture(buffer);
	sprintf(buffer, "GUIComp num: %d", GUIComps.size());
	GUICount->ReloadTexture(buffer);
	sprintf(buffer, "SolidObj num: %d", solidObjs.size());
	SolidCount->ReloadTexture(buffer);
	sprintf(buffer, "Pos: %f, %f, %f", character->GetBox().GetPosition().x,
                                       character->GetBox().GetPosition().y,
                                       character->GetBox().GetPosition().z - character->GetBox().GetHalfSize().z);
	posValue->ReloadTexture(buffer);
	sprintf(buffer, "frontObjs num: %d, %d, %d, %d", character->frontObjs.size(),
                                                     char2->frontObjs.size(),
                                                     char3->frontObjs.size(),
                                                     char4->frontObjs.size());
	charsFrontObjsNum->ReloadTexture(buffer);
	sprintf(buffer, "grounded: %s, %d, %d, %s, %d, %d, %f, %d", character->IsGrounded() ? "true" : "false",
                                                character->GetBox().GetContactObjs().size(),
                                                pWorld.GetParticles().size(),
                                                inputHandler->MouseLeftDown() ? "left pressed" : "not pressed",
                                                pressedGUIComp,
                                                selectedGUIComp,
                                                character->GetBox().GetMotion(),
                                                phyStepCount);
	resortCount->ReloadTexture(buffer);
}

DBG_Status Scene1::ResortSolidObjs()
{
    resortCt++;

    return Scene::ResortSolidObjs();
}

}
