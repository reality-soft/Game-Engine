#include "TestGame.h"

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ENGINE->OnInit(hinstance, L"Game-Engine", { 1920, 1080 });

	TestGame test_game;

	ENGINE->Run(&test_game);

	ENGINE->OnRelease();

	return 0;
}