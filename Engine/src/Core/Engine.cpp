#include "stdafx.h"
#include "Engine.h"
#include "ResourceMgr.h"
#include "GUIMgr.h"
#include "EventMgr.h"
#include "InputEventMgr.h"
#include "SceneMgr.h"
#include "FmodMgr.h"
#include "WriteMgr.h"

#ifdef _DEBUG
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

LRESULT CALLBACK WindowProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
#endif

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		//ENGINE->OnResized();
		break;

#ifdef _DEBUG
	case WM_DPICHANGED:
		if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
#endif
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace reality {
	bool Engine::OnInit(HINSTANCE hinstance, LPCWSTR title, E_Resolution resolution, bool titlebar)
	{
		POINT screen_size = { 0, 0 };
		
		current_resolution = resolution;

		screen_size = E_Resolution_Size[resolution];

		// À©µµ¿ì ÃÊ±âÈ­
		if (InitWindow(hinstance, title, screen_size, titlebar) == false)
			return false;

		// DX ÃÊ±âÈ­
		if (DX11APP->OnInit(screen_size, hwnd) == false)
			return false;

#ifdef _DEBUG
		GUI->Init(ENGINE->GetWindowHandle(), DX11APP->GetDevice(), DX11APP->GetDeviceContext());
#endif		
		DINPUT->Init();
		TIMER->Init();
		FMOD_MGR->Init();
		WRITER->Init();

		return true;
	}

	void Engine::Run()
	{
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
			DINPUT->Update();
			INPUT_EVENT->PollEvents();
			SCENE_MGR->OnUpdate();
			EVENT->ProcessEvents();
			FMOD_MGR->Update();

			// Render Here
			DX11APP->PreRender(true, true, true);

			SCENE_MGR->OnRender();

			DX11APP->PostRender(false);
		}
	}

	void Engine::OnResized()
	{
		RECT new_rc;
		GetClientRect(hwnd, &new_rc);
		wnd_size.x = new_rc.right - new_rc.left;
		wnd_size.y = new_rc.bottom - new_rc.top;

		DX11APP->Resize(wnd_size.x, wnd_size.y);
	}

	void Engine::OnRelease()
	{
		FMOD_MGR->Release();
		DX11APP->OnRelease();
	}

	void Engine::Resize(E_Resolution new_resolution)
	{
		current_resolution = new_resolution;

		POINT screen_size = E_Resolution_Size[new_resolution];

		int x = GetSystemMetrics(SM_CXSCREEN) / 2 - screen_size.x / 2;
		int y = GetSystemMetrics(SM_CYSCREEN) / 2 - screen_size.y / 2;

		SetWindowPos(hwnd, NULL, x, y, screen_size.x, screen_size.y, SWP_NOZORDER | SWP_FRAMECHANGED);

		RECT new_rc;
		GetClientRect(hwnd, &new_rc);
		wnd_size.x = new_rc.right - new_rc.left;
		wnd_size.y = new_rc.bottom - new_rc.top;

		DX11APP->Resize(wnd_size.x, wnd_size.y);

	}

	bool Engine::InitWindow(HINSTANCE hinstance, LPCWSTR title, POINT screen_size, bool titlebar)
	{
		this->hinstance = hinstance;
		this->title = title;

		auto style = titlebar ? WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME :  WS_POPUP;

		RECT rc = { 0, 0, screen_size.x, screen_size.y };
		AdjustWindowRect(&rc, style, false);
		this->wnd_size.x = rc.right - rc.left;
		this->wnd_size.y = rc.bottom - rc.top;

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
			style,
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

		return true;
	}
}

