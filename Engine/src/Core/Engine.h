#include "DX11App.h"

namespace reality {

	enum E_Resolution
	{
		RPOP = 0,
		R1920x1080 = 1,
		R1280x720 = 2,
		RESOLUTION_COUNT,
	};

	static const string E_Resolution_String[] = { "POP", "1920 X 1080", "1280 X 720" };
	static POINT  E_Resolution_Size[] = { { 600, 400 }, { 1920, 1080 }, { 1280, 720 } };
	static const float  E_Resolution_Multiply[] = { 1.0f, 1.0f, 2.0f / 3.0f };

	class DLL_API Engine
	{
		SINGLETON(Engine)
#define ENGINE reality::Engine::GetInst()

	public:
		bool OnInit(HINSTANCE hinstance, LPCWSTR title, E_Resolution resolution, bool titlebar);
		void Run();
		void OnResized();
		void OnRelease();
		void Resize(E_Resolution new_resolution);

	private:
		HINSTANCE		hinstance;
		LPCWSTR			title;
		HWND			hwnd;
		POINT			wnd_size;
		E_Resolution    current_resolution;

	public:
		POINT			GetWindowSize() { return wnd_size; }
		HINSTANCE		GetInstanceHandle() { return hinstance; }
		HWND			GetWindowHandle() { return hwnd; }
		E_Resolution	GetWindowResolution() { return current_resolution; }
	private:
		bool InitWindow(HINSTANCE hinstance, LPCWSTR title, POINT screen_size, bool titlebar);
	};
}


