#pragma once

#ifdef PLATFORM_WINDOWS

extern KGCA41B::Engine* KGCA41B::CreateEngine();

int main(int argc, char** argv) {
	auto engine = KGCA41B::CreateEngine();
	engine->Run();
	delete engine;
}

#endif