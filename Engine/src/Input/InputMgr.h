#pragma once
  
namespace reality
{
	enum KeyState
	{
		KEY_FREE = 0,
		KEY_UP,
		KEY_PUSH,
		KEY_HOLD,
	};

	enum MouseButton
	{
		L_BUTTON,
		R_BUTTON,
		M_BUTTON,
		EXTRA_BUTTON,
		NUM_MOUSE_BUTTON
	};

	class DLL_API InputMgr
	{
		SINGLETON(InputMgr);
#define DINPUT reality::InputMgr::GetInst()
	public:
		bool Init();
		bool Update();

		KeyState GetKeyState(DWORD input_key);
		KeyState GetMouseState(MouseButton input_key);

		POINT GetMousePosition();
		XMFLOAT2 GetMouseVelocity();
		int GetMouseWheel();
		int GetDeltaX();
		int GetDeltaY();

		void Active(bool shoud_active);

		bool is_active = true;

	private:
		ComPtr<IDirectInput8> dinput;
		ComPtr<IDirectInputDevice8> di_keyboard;
		ComPtr<IDirectInputDevice8> di_mouse;

		UCHAR keyboard_state[256];
		KeyState prev_keyboard_state[256];

		DIMOUSESTATE mouse_state;
		KeyState  prev_rgb_buttons[4];

		POINT current_mousepos;

	};

}
