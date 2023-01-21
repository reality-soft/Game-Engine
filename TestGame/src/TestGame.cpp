#include <Engine_include.h>


int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ENGINE->OnInit(hinstance, L"Game-Engine", {1920, 1080});

	ENGINE->Run();

	return 0;
}