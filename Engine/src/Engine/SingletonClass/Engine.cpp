#include "Engine.h"
#include "ResourceMgr.h"

LRESULT CALLBACK WindowProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		ENGINE->OnResized();
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace KGCA41B {
	bool Engine::OnInit(HINSTANCE hinstance, LPCWSTR title, POINT wnd_size)
	{
		// 윈도우 초기화
		if (InitWindow(hinstance, title, wnd_size) == false)
			return false;

		// DX 초기화
		if (DX11APP->OnInit(wnd_size, hwnd) == false)
			return false;

		return true;
	}

	void Engine::Run(Scene* scene)
	{
		TIMER->Init();
		scene->OnInit();

		bool done = false;
		while (!done)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			if (done)
				break;

			// Updates
			TIMER->Update();
			scene->OnUpdate();

			DX11APP->PreRender(true, true, true);

			// Render Here
			scene->OnRender();

			DX11APP->PostRender(false);
		}
		scene->OnRelease();
	}

	void Engine::OnResized()
	{
		RECT new_rc;
		GetClientRect(hwnd, &new_rc);
		wnd_size.x = new_rc.right  - new_rc.left;
		wnd_size.y = new_rc.bottom - new_rc.top;

		DX11APP->Resize(wnd_size.x, wnd_size.y);
	}

	void Engine::OnRelease()
	{
		DX11APP->OnRelease();
	}

	bool Engine::InitWindow(HINSTANCE hinstance, LPCWSTR title, POINT wnd_size)
	{
		this->hinstance = hinstance;
		this->title = title;
		this->wnd_size = wnd_size;

		WNDCLASS window_class;
		ZeroMemory(&window_class, sizeof(window_class));

		window_class.hInstance = hinstance;
		window_class.lpszClassName = title;
		window_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = WindowProc;

		if (RegisterClass(&window_class) == false)
			return false;

		hwnd = CreateWindow(
			window_class.lpszClassName,
			title,
			WS_OVERLAPPEDWINDOW,
			GetSystemMetrics(SM_CXSCREEN) / 2 - wnd_size.x / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - wnd_size.y / 2,
			wnd_size.x, wnd_size.y,
			NULL, NULL,
			hinstance,
			NULL);

		if (hwnd == nullptr)
			return false;

		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);

		RECT rc;
		GetClientRect(hwnd, &rc);
		wnd_size.x = rc.right - rc.left;
		wnd_size.y = rc.bottom - rc.top;

		return true;
	}
}

