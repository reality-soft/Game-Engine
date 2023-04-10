#include "DX11App.h"
#include "UI.h"

namespace reality {
	enum E_Resolution
	{
		R1920x1080 = 0,
		R1280x720 = 1,
		RESOLUTION_COUNT,
	};

	static const string E_Resolution_String[] = { "1920 X 1080", "1280 X 720" };

	class DLL_API Engine
	{
		SINGLETON(Engine)
#define ENGINE reality::Engine::GetInst()

	public:
		bool OnInit(HINSTANCE hinstance, LPCWSTR title, E_Resolution resolution);
		void Run();
		void OnResized();
		void OnRelease();
		void Resize(E_Resolution new_resolution);

	public:
		POINT			GetWindowSize() { return wnd_size; }
		HINSTANCE		GetInstanceHandle() { return hinstance; }
		HWND			GetWindowHandle() { return hwnd; }
		E_Resolution	GetWindowResolution() { return current_resolution; }
	private:
		bool InitWindow(HINSTANCE hinstance, LPCWSTR title, POINT screen_size);

	private:
		HINSTANCE		hinstance;
		LPCWSTR			title;
		HWND			hwnd;
		POINT			wnd_size;
		E_Resolution    current_resolution;

	};
}


