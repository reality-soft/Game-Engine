#include "Engine_include.h"

class TestGame : public KGCA41B::Engine {

};

int main() {
	TestGame* testGame = new TestGame;

	testGame->Run();

	delete testGame;
}