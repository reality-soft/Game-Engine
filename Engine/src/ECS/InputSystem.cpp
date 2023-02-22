#include "stdafx.h"
#include "InputSystem.h"

using namespace KGCA41B;

void InputSystem::OnCreate(entt::registry& reg)
{
}

void InputSystem::OnUpdate(entt::registry& reg)
{
	int result = DINPUT->Update();

	auto view_inp = reg.view<C_InputMapping>();
	for (auto ent : view_inp)
	{
		auto& input_mapping = view_inp.get<C_InputMapping>(ent);
		Clear(input_mapping);

		if (result != MOUSE_NO_STATE)
		{
			if (DINPUT->GetMouseButton().y)
			{
				XMFLOAT2 mouse_velocity = DINPUT->GetMouseVelocity();

				input_mapping.axis_types.push_back(AxisType::YAW);
				input_mapping.axis_value[(int)AxisType::YAW] = mouse_velocity.x * 0.1f;

				input_mapping.axis_types.push_back(AxisType::PITCH);
				input_mapping.axis_value[(int)AxisType::PITCH] = mouse_velocity.y * 0.1f;				
			}
			if (DINPUT->GetMouseButton().x)
			{
				input_mapping.actions.push_back(ActionType::ATTACK);
			}
		}

		if (result != KEY_NO_STATE)
		{
			if (DINPUT->IsKeyPressed(DIK_W))
			{
				input_mapping.axis_types.push_back(AxisType::FROWARD);
				input_mapping.axis_value[(int)AxisType::FROWARD] = 1;
			}
			if (DINPUT->IsKeyPressed(DIK_A))
			{
				input_mapping.axis_types.push_back(AxisType::RIGHT);
				input_mapping.axis_value[(int)AxisType::RIGHT] = 1;
			}
			if (DINPUT->IsKeyPressed(DIK_S))
			{
				input_mapping.axis_types.push_back(AxisType::FROWARD);
				input_mapping.axis_value[(int)AxisType::FROWARD] = -1;
			}
			if (DINPUT->IsKeyPressed(DIK_D))
			{
				input_mapping.axis_types.push_back(AxisType::RIGHT);
				input_mapping.axis_value[(int)AxisType::RIGHT] = -1;
			}
			if (DINPUT->IsKeyPressed(DIK_Q))
			{
				input_mapping.axis_types.push_back(AxisType::UP);
				input_mapping.axis_value[(int)AxisType::UP] = -1;
			}
			if (DINPUT->IsKeyPressed(DIK_E))
			{
				input_mapping.axis_types.push_back(AxisType::UP);
				input_mapping.axis_value[(int)AxisType::UP] = +1;
			}
		}
	}
}

void KGCA41B::InputSystem::Clear(C_InputMapping& input_mapping)
{
	input_mapping.actions.clear();
	input_mapping.axis_types.clear();
	for (auto& value : input_mapping.axis_value)
		value = 0;
}

