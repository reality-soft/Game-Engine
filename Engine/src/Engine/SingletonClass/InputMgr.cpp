#include "InputMgr.h"
#include "Engine.h"

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

int InputMgr::Update()
{
	HRESULT hr;

	hr = di_keyboard.Get()->GetDeviceState(sizeof(keyboard_state), (LPVOID)&keyboard_state);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			di_keyboard->Acquire();
		else
			return KEY_NO_STATE;
	}

	hr = di_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			di_mouse->Acquire();
		else
			return MOUSE_NO_STATE;
	}

	current_mousepos.x += mouse_state.lX;
	current_mousepos.y += mouse_state.lY;

	current_mousepos.x = max(current_mousepos.x, 0);
	current_mousepos.x = min(current_mousepos.x, screen_size.x);
	current_mousepos.y = max(current_mousepos.y, 0);
	current_mousepos.y = min(current_mousepos.y, screen_size.y);

	return -1;
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
	return current_mousepos;
}

XMFLOAT2 InputMgr::GetMouseVelocity()
{
	return XMFLOAT2(mouse_state.lX, mouse_state.lY);
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
