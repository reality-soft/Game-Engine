#include "stdafx.h"
#include "InputMgr.h"
#include "Engine.h"

using namespace reality;

bool InputMgr::Init()
{
	HRESULT hr;
	hr = DirectInput8Create(ENGINE->GetInstanceHandle(), DIRECTINPUT_HEADER_VERSION, IID_IDirectInput8, (void**)dinput.GetAddressOf(), nullptr);

	hr = dinput.Get()->CreateDevice(GUID_SysKeyboard, di_keyboard.GetAddressOf(), nullptr);
	hr = di_keyboard.Get()->SetDataFormat(&c_dfDIKeyboard);
	hr = di_keyboard.Get()->SetCooperativeLevel(ENGINE->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	hr = di_keyboard.Get()->Acquire();

	hr = dinput.Get()->CreateDevice(GUID_SysMouse, di_mouse.GetAddressOf(), nullptr);
	hr = di_mouse.Get()->SetDataFormat(&c_dfDIMouse);
	hr = di_mouse.Get()->SetCooperativeLevel(ENGINE->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	hr = di_mouse.Get()->Acquire();

	for (int i = 0; i < 256; i++) {
		prev_keyboard_state[i] = KEY_FREE;
	}

	for (int i = 0; i < 4; i++) {
		prev_rgb_buttons[i] = KEY_FREE;
	}

	return true;
}

bool InputMgr::Update()
{
	if (is_active == false)
	{
		di_keyboard->Unacquire();
		di_mouse->Unacquire();
		return false;
	}

	HRESULT hr = di_keyboard.Get()->GetDeviceState(sizeof(keyboard_state), (LPVOID)&keyboard_state);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			di_keyboard->Acquire();
		else
			return false;
	}

	for (int i = 0; i < 256; i++) {
		if (keyboard_state[i] & 0x80) {
			if (prev_keyboard_state[i] == KEY_FREE || prev_keyboard_state[i] == KEY_UP) {
				prev_keyboard_state[i] = KEY_PUSH;
			}
			else {
				prev_keyboard_state[i] = KEY_HOLD;
			}
		}
		else {
			if (prev_keyboard_state[i] == KEY_HOLD || prev_keyboard_state[i] == KEY_PUSH) {
				prev_keyboard_state[i] = KEY_UP;
			}
			else {
				prev_keyboard_state[i] = KEY_FREE;
			}
		}
	}

	hr = di_mouse.Get()->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			di_mouse->Acquire();
		else
			return false;
	}

	for (int i = 0; i < 4; i++) {
		if (mouse_state.rgbButtons[i] & 0x80) {
			if (prev_rgb_buttons[i] == KEY_FREE || prev_rgb_buttons[i] == KEY_UP) {
				prev_rgb_buttons[i] = KEY_PUSH;
			}
			else {
				prev_rgb_buttons[i] = KEY_HOLD;
			}
		}
		else {
			if (prev_rgb_buttons[i] == KEY_HOLD || prev_rgb_buttons[i] == KEY_PUSH) {
				prev_rgb_buttons[i] = KEY_UP;
			}
			else {
				prev_rgb_buttons[i] = KEY_FREE;
			}
		}
	}

	::GetCursorPos(&current_mousepos);
	::ScreenToClient(ENGINE->GetWindowHandle(), &current_mousepos);

	return true;
}

KeyState reality::InputMgr::GetKeyState(DWORD input_key)
{
	return prev_keyboard_state[input_key];
}

KeyState reality::InputMgr::GetMouseState(MouseButton input_key)
{
	return prev_rgb_buttons[input_key];
}

POINT InputMgr::GetMousePosition()
{
	return current_mousepos;
}

XMFLOAT2 InputMgr::GetMouseVelocity()
{
	return XMFLOAT2(mouse_state.lX, mouse_state.lY);
}

int InputMgr::GetMouseWheel()
{
	if (is_active == false)
		return mouse_state.lZ = 0;

	return mouse_state.lZ;
}

int reality::InputMgr::GetDeltaX()
{
	return mouse_state.lX;
}

int reality::InputMgr::GetDeltaY()
{
	return mouse_state.lY;
}

void reality::InputMgr::Active(bool shoud_active)
{
	is_active = shoud_active;
}
