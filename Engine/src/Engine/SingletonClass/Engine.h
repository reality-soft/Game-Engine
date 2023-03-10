#include "DX11App.h"

namespace reality {
	class DLL_API Engine
	{
		SINGLETON(Engine)
#define ENGINE reality::Engine::GetInst()

	public:
		bool OnInit(HINSTANCE hinstance, LPCWSTR title, POINT screen_size);
		void Run();
		void OnResized();
		void OnRelease();

	public:
		POINT     GetWindowSize() { return wnd_size; }
		HINSTANCE GetInstanceHandle() { return hinstance; }
		HWND      GetWindowHandle() { return hwnd; }

	private:
		bool InitWindow(HINSTANCE hinstance, LPCWSTR title, POINT screen_size);

	private:
		HINSTANCE hinstance;
		LPCWSTR   title;
		HWND      hwnd;
		POINT     wnd_size;

	};
}


