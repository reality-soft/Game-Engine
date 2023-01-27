#pragma once
#include "common.h"
#include "DllMacro.h"
#include <dinput.h>

namespace KGCA41B
{

	class DLL_API InputMgr
	{
		SINGLETON(InputMgr);
	public:
		bool Init(HWND hwnd, HINSTANCE hinstacne);
		void Update();

		bool IsKeyPressed(UCHAR dik);
		int  GetKeyEvent(UCHAR dik);

		POINT GetMousePosition();
		POINT GetMouseVelocity();
		XMINT3 GetMouseButton();
		int GetMouseWheel();

	private:
		ComPtr<IDirectInput8> dinput;
		ComPtr<IDirectInputDevice8> di_keyboard;
		ComPtr<IDirectInputDevice8> di_mouse;

		UCHAR keyboard_state[256];
		DIMOUSESTATE mouse_state;
		POINT screen_size;
		POINT mouse_position;

	};

}
