#include "TestGame.h"

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ENGINE->OnInit(hinstance, L"Game-Engine", { 1920, 1080 });

	SCENE_MGR->SetScene<TestGame>();
	ENGINE->Run();

	ENGINE->OnRelease();

	return 0;
}