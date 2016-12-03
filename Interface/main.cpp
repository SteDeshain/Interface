#include "game.h"
#include "interface_game.h"

using namespace steg;
using namespace gameTest;

int main(int argc, char *argv[])
{
    Game::EngineInit();

    InterfaceGame* interfaceGame = new InterfaceGame();

    interfaceGame->Run();

    Game::EngineQuit();

	return 0;
}
