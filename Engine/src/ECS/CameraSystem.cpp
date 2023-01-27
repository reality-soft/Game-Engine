#include "CameraSystem.h"
#include "TimeMgr.h"

using namespace KGCA41B;

CameraSystem::CameraSystem()
{
	camera = nullptr;
	viewport = DX11APP->GetViewPortAddress();
	view_matrix = XMMatrixIdentity();
	projection_matrix = XMMatrixIdentity();
}

CameraSystem::~CameraSystem()
{
	viewport = nullptr;
}

void CameraSystem::OnCreate(entt::registry& reg, string tag)
{
	auto view = reg.view<Camera>();
	for (auto entity : view)
	{
		auto& camera = view.get<Camera>(entity);
		if (camera.tag == tag)
		{
			this->camera = &camera;
		}
	}

	view_matrix = XMMatrixLookAtLH(camera->position, camera->target, camera->up);
}

void CameraSystem::OnUpdate(entt::registry& reg)
{
	auto view_input = reg.view<InputMapping>();
	auto view_camera = reg.view<Camera>();
	auto view_trans = reg.view<Transform>();

	for (auto ent : view_input)
	{
		auto& input = view_input.get<InputMapping>(ent);
		
		CameraMovement(input);
	}

	CreateMatrix();

	for (auto ent : view_trans)
	{
		auto& trans = view_trans.get<Transform>(ent);
		trans.view_matrix = view_matrix;
		trans.projection_matrix = projection_matrix;
	}
}

void CameraSystem::CameraMovement(InputMapping& input_mapping)
{
	XMVECTOR front_dir = XMVectorScale(camera->look, camera->speed * TM_DELTATIME);
	XMVECTOR right_dir = XMVectorScale(camera->right, camera->speed * TM_DELTATIME);
	XMVECTOR up_dir = XMVectorScale(camera->up, camera->speed * TM_DELTATIME);

	switch (input_mapping.axis_type)
	{
	case AxisType::IDLE:
		break;

	case AxisType::FROWARD:
		camera->position += XMVectorScale(front_dir, input_mapping.axis_value[(int)AxisType::FROWARD]);
		camera->target   += XMVectorScale(front_dir, input_mapping.axis_value[(int)AxisType::FROWARD]);

	case AxisType::RIGHT:
		camera->position += XMVectorScale(right_dir, input_mapping.axis_value[(int)AxisType::RIGHT]);
		camera->target   += XMVectorScale(right_dir, input_mapping.axis_value[(int)AxisType::RIGHT]);

	case AxisType::UP:
		camera->position += XMVectorScale(up_dir, input_mapping.axis_value[(int)AxisType::UP]);
		camera->target   += XMVectorScale(up_dir, input_mapping.axis_value[(int)AxisType::UP]);

	case AxisType::YAW:
		camera->yaw += input_mapping.axis_value[(int)AxisType::YAW];

	case AxisType::PITCH:
		camera->pitch += input_mapping.axis_value[(int)AxisType::PITCH];

	case AxisType::ROLL:
		camera->roll += input_mapping.axis_value[(int)AxisType::ROLL];
	}
}

void CameraSystem::CreateMatrix()
{
	XMMATRIX world, view;
	XMVECTOR s, o, q, t;
	XMFLOAT3 position(camera->position.m128_f32[0], camera->position.m128_f32[1], camera->position.m128_f32[2]);

	s = XMVectorReplicate(1.0f);
	o = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	q = DirectX::XMQuaternionRotationRollPitchYaw(camera->pitch, camera->yaw, camera->roll);
	t = DirectX::XMLoadFloat3(&position);
	world = DirectX::XMMatrixAffineTransformation(s, o, q, t);
	view  = DirectX::XMMatrixInverse(0, world);

	camera->look = XMVector3Normalize(XMVector3TransformCoord(XMVectorSet(0, 0, -1, 0), view));
	camera->right = XMVector3Normalize(XMVector3TransformCoord(XMVectorSet(1, 0, 0, 0), view));
	camera->up = XMVector3Normalize(XMVector3TransformCoord(XMVectorSet(0, 1, 0, 0), view));

	view_matrix = view;
}
