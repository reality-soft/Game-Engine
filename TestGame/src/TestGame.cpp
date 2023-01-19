#include <Engine_include.h>

class TestGame : public KGCA41B::Engine {

};

KGCA41B::Engine* KGCA41B::CreateEngine() {
	return new Engine();
}