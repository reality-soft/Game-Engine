#include "Engine.h"
#include "ResourceMgr.h"
#include "GUIMgr.h"
#include "PhysicsMgr.h"
#include "../Engine/../SpacePartition/SpacePartition.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		ENGINE->OnResized();
		break;

	case WM_DPICHANGED:
		if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
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

		GUI->Init(ENGINE->GetWindowHandle(), DX11APP->GetDevice(), DX11APP->GetDeviceContext());
		PHYSICS->Init();
		TIMER->Init();

		SpacePartition::GetInst()->Init({ 300, 300 });

		return true;
	}

	void Engine::Run(Scene* scene)
	{
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
			PHYSICS->Update();

			scene->OnUpdate();


			// Render Here
			DX11APP->PreRender(true, true, true);

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

