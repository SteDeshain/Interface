#include "game.h"
#include "game1.h"

using namespace steg;
using namespace gameTest;

int main(int argc, char *argv[])
{
    Game::EngineInit();

    Game1* game1 = new Game1();

    game1->Run();

    Game::EngineQuit();

	return 0;
}
