#include "stdafx.h"
#include "CameraSystem.h"
#include "TimeMgr.h"
#include "InputMgr.h"
#include "Engine.h"
#include "PhysicsMgr.h"

using namespace KGCA41B;

CameraSystem::CameraSystem()
{
	camera = nullptr;
	viewport = DX11APP->GetViewPortAddress();

	world_matrix = XMMatrixIdentity();
	view_matrix = cb_viewproj.data.view_matrix = XMMatrixIdentity();
	projection_matrix = cb_viewproj.data.projection_matrix = XMMatrixIdentity();
}

CameraSystem::~CameraSystem()
{
	viewport = nullptr;
}

void KGCA41B::CameraSystem::TargetTag(entt::registry& reg, string tag)
{
	auto view = reg.view<C_Camera>();
	for (auto entity : view)
	{
		auto& camera = view.get<C_Camera>(entity);
		if (camera.tag == tag)
		{
			this->camera = &camera;
		}
	}
}

void CameraSystem::OnCreate(entt::registry& reg)
{
	if (camera != nullptr)
		cb_viewproj.data.view_matrix = XMMatrixLookAtLH(camera->position, camera->target, camera->up);

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbViewProj::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = cb_viewproj.buffer.GetAddressOf();

	HRESULT hr;
	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_viewproj.buffer.GetAddressOf());
}

void CameraSystem::OnUpdate(entt::registry& reg)
{
	auto view_input = reg.view<C_InputMapping>();
	auto view_trans = reg.view<C_Transform>();

	for (auto ent : view_input)
	{
		auto& input = view_input.get<C_InputMapping>(ent);
		
		//CameraMovement(input);
		CameraAction(input);
	}

	CreateMatrix();

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_viewproj.buffer.Get(), 0, nullptr, &cb_viewproj.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_viewproj.buffer.GetAddressOf());
}

MouseRay CameraSystem::CreateMouseRay()
{
	MouseRay mouse_ray;
	POINT cursor_pos;
	GetCursorPos(&cursor_pos);
	ScreenToClient(ENGINE->GetWindowHandle(), &cursor_pos);

	// Convert the mouse position to a direction in world space
	float mouse_x = static_cast<float>(cursor_pos.x);
	float mouse_y = static_cast<float>(cursor_pos.y);

	float ndc_x =   (2.0f * mouse_x / (float)ENGINE->GetWindowSize().x - 1.0f) / projection_matrix.r[0].m128_f32[0];
	float ndc_y =  (-2.0f * mouse_y / (float)ENGINE->GetWindowSize().y + 1.0f) / projection_matrix.r[1].m128_f32[1];

	ndc.x = ndc_x;
	ndc.y = ndc_y;	

	XMMATRIX inv_view = XMMatrixInverse(nullptr, view_matrix);
	XMMATRIX inv_world = XMMatrixInverse(nullptr, XMMatrixIdentity());

	XMVECTOR ray_dir;
	XMVECTOR ray_origin;


	ray_origin = XMVector3TransformCoord({0, 0, 0, 0}, inv_view);
	ray_dir = XMVector3TransformNormal({ndc_x, ndc_y, 1.0f, 0}, inv_view);
	ray_dir = XMVector3Normalize(ray_dir);

	XMtoRP(ray_origin, mouse_ray.start_point);
	XMtoRP(ray_dir * camera->far_z * 10, mouse_ray.end_point);

	return mouse_ray;
}

C_Camera* CameraSystem::GetCamera()
{
	return camera;
}

XMMATRIX KGCA41B::CameraSystem::GetViewProj()
{
	return view_matrix * projection_matrix;
}

void CameraSystem::CameraMovement(C_InputMapping& input_mapping)
{
	XMVECTOR front_dir = camera->look * camera->speed * TM_DELTATIME;
	XMVECTOR right_dir = camera->right * camera->speed * TM_DELTATIME * -1.f;
	XMVECTOR up_dir = camera->up * camera->speed * TM_DELTATIME;

	for (auto axis_type : input_mapping.axis_types)
	{
		switch (axis_type)
		{
		case AxisType::IDLE:
			break;

		case AxisType::FROWARD:
			camera->position += front_dir * input_mapping.axis_value[(int)AxisType::FROWARD];
			camera->look += front_dir * input_mapping.axis_value[(int)AxisType::FROWARD];

		case AxisType::RIGHT:
			camera->position += right_dir * input_mapping.axis_value[(int)AxisType::RIGHT];
			camera->look += right_dir * input_mapping.axis_value[(int)AxisType::RIGHT];

		case AxisType::UP:
			camera->position += up_dir * input_mapping.axis_value[(int)AxisType::UP];
			camera->look += up_dir * input_mapping.axis_value[(int)AxisType::UP];

		case AxisType::YAW:
			camera->yaw += input_mapping.axis_value[(int)AxisType::YAW] * TM_DELTATIME * 5;

		case AxisType::PITCH:
			camera->pitch += input_mapping.axis_value[(int)AxisType::PITCH] * TM_DELTATIME * 5;

		case AxisType::ROLL:
			camera->roll += input_mapping.axis_value[(int)AxisType::ROLL] * TM_DELTATIME * 5;
		}
	}
}

void CameraSystem::CameraAction(C_InputMapping& input_mapping)
{
	for (auto actions : input_mapping.actions)
	{
		switch (actions)
		{
		case ActionType::ATTACK:
		{

		} break;
		}
	}
}

void CameraSystem::CreateMatrix()
{
	camera->aspect = viewport->Width / viewport->Height;
	projection_matrix = XMMatrixPerspectiveFovLH(camera->fov, camera->aspect, camera->near_z, camera->far_z);

	XMVECTOR s, o, q, t;
	XMFLOAT3 position(camera->position.m128_f32[0], camera->position.m128_f32[1], camera->position.m128_f32[2]);

	s = XMVectorReplicate(1.0f);
	o = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	q = XMQuaternionRotationRollPitchYaw(camera->pitch, camera->yaw, camera->roll);
	t = XMLoadFloat3(&position);
	world_matrix = XMMatrixAffineTransformation(s, o, q, t);
	view_matrix = XMMatrixInverse(0, world_matrix);

	camera->look = XMVector3Normalize(XMMatrixTranspose(view_matrix).r[2]);
	camera->right = XMVector3Normalize(XMMatrixTranspose(view_matrix).r[0]);
	camera->up = XMVector3Normalize(XMMatrixTranspose(view_matrix).r[1]);
	camera->position = world_matrix.r[3];


	cb_viewproj.data.view_matrix = XMMatrixTranspose(view_matrix);
	cb_viewproj.data.projection_matrix = XMMatrixTranspose(projection_matrix);
}