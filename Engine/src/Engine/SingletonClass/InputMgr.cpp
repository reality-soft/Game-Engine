#include "InputMgr.h"

using namespace KGCA41B;

bool InputMgr::Init(HWND hwnd, HINSTANCE hinstacne)
{
	RECT crect;
	GetClientRect(hwnd, &crect);
	screen_size = { crect.right, crect.bottom };

	HRESULT hr;
	hr = DirectInput8Create(hinstacne, DIRECTINPUT_HEADER_VERSION, IID_IDirectInput8, (void**)dinput.GetAddressOf(), nullptr);

	// 키보드
	hr = dinput.Get()->CreateDevice(GUID_SysKeyboard, di_keyboard.GetAddressOf(), nullptr);

	hr = di_keyboard.Get()->SetDataFormat(&c_dfDIKeyboard);

	hr = di_keyboard.Get()->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = di_keyboard.Get()->Acquire();

	// 마우스
	hr = dinput.Get()->CreateDevice(GUID_SysMouse, di_mouse.GetAddressOf(), nullptr);

	hr = di_mouse.Get()->SetDataFormat(&c_dfDIMouse);

	hr = di_mouse.Get()->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = di_mouse.Get()->Acquire();

	return true;
}

void InputMgr::Update()
{
	HRESULT hr;

	hr = di_keyboard.Get()->GetDeviceState(sizeof(keyboard_state), (LPVOID)&keyboard_state);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			di_keyboard->Acquire();
		else
			return;
	}

	hr = di_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			di_mouse->Acquire();
		else
			return;
	}
}

bool InputMgr::IsKeyPressed(UCHAR dik)
{
	if (keyboard_state[dik] & 0x80)
		return true;

	return false;
}

int InputMgr::GetKeyEvent(UCHAR dik)
{
	DIDEVICEOBJECTDATA key_data[256];
	DWORD buffer_size = ARRAYSIZE(key_data);

	if (FAILED(di_keyboard.Get()->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), key_data, &buffer_size, 0)))
		return 0;

	if (key_data[dik].dwData & 0x80)
	{
		// key down event;
		return 1;
	}
	else if (key_data[dik].dwData == 0)
	{
		// key up event
		return -1;
	}
	return 0;
}

POINT InputMgr::GetMousePosition()
{
	POINT mousepos;
	mousepos.x = mouse_state.lX;
	mousepos.y = mouse_state.lY;

	mousepos.x = max(mousepos.x, 0);
	mousepos.x = min(mousepos.x, screen_size.x);
	mousepos.y = max(mousepos.y, 0);
	mousepos.y = min(mousepos.y, screen_size.y);

	return mousepos;
}

POINT InputMgr::GetMouseVelocity()
{
	static POINT lastpos = { 0, };
	POINT curpos = GetMousePosition();
	POINT velocity = { curpos.x - lastpos.x, curpos.y - lastpos.y };
	lastpos = curpos;

	return velocity;
}

XMINT3 InputMgr::GetMouseButton()
{
	XMINT3 buttons = { 0, 0, 0 };
	if (mouse_state.rgbButtons[0] & 0x80) // L
		buttons.x = 1;

	if (mouse_state.rgbButtons[1] & 0x80) // R
		buttons.y = 1;

	if (mouse_state.rgbButtons[2] & 0x80) // M
		buttons.z = 1;

	return buttons;
}

int InputMgr::GetMouseWheel()
{
	return mouse_state.lZ;
}
