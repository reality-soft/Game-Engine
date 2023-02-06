#include "CameraSystem.h"
#include "TimeMgr.h"

using namespace KGCA41B;

CameraSystem::CameraSystem()
{
	camera = nullptr;
	viewport = DX11APP->GetViewPortAddress();
	cb_viewproj.data.view_matrix = XMMatrixIdentity();
	cb_viewproj.data.projection_matrix = XMMatrixIdentity();
}

CameraSystem::~CameraSystem()
{
	viewport = nullptr;
}

void KGCA41B::CameraSystem::TargetTag(entt::registry& reg, string tag)
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
	auto view_input = reg.view<InputMapping>();
	auto view_trans = reg.view<Transform>();

	for (auto ent : view_input)
	{
		auto& input = view_input.get<InputMapping>(ent);
		
		CameraMovement(input);
	}

	CreateMatrix();

	cb_viewproj.data.view_matrix = XMMatrixTranspose(cb_viewproj.data.view_matrix);
	cb_viewproj.data.projection_matrix = XMMatrixTranspose(cb_viewproj.data.projection_matrix);

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_viewproj.buffer.Get(), 0, nullptr, &cb_viewproj.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_viewproj.buffer.GetAddressOf());
}

void CameraSystem::CameraMovement(InputMapping& input_mapping)
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
			camera->target += front_dir * input_mapping.axis_value[(int)AxisType::FROWARD];

		case AxisType::RIGHT:
			camera->position += right_dir * input_mapping.axis_value[(int)AxisType::RIGHT];
			camera->target += right_dir * input_mapping.axis_value[(int)AxisType::RIGHT];

		case AxisType::UP:
			camera->position += up_dir * input_mapping.axis_value[(int)AxisType::UP];
			camera->target += up_dir * input_mapping.axis_value[(int)AxisType::UP];

		case AxisType::YAW:
			camera->yaw += input_mapping.axis_value[(int)AxisType::YAW] * TM_DELTATIME;

		case AxisType::PITCH:
			camera->pitch += input_mapping.axis_value[(int)AxisType::PITCH] * TM_DELTATIME;

		case AxisType::ROLL:
			camera->roll += input_mapping.axis_value[(int)AxisType::ROLL] * TM_DELTATIME;
		}
	}
}

void CameraSystem::CreateMatrix()
{
	camera->aspect = viewport->Width / viewport->Height;
	cb_viewproj.data.projection_matrix = XMMatrixPerspectiveFovLH(camera->fov, camera->aspect, camera->near_z, camera->far_z);

	XMMATRIX world, view;
	XMVECTOR s, o, q, t;
	XMFLOAT3 position(camera->position.m128_f32[0], camera->position.m128_f32[1], camera->position.m128_f32[2]);

	s = XMVectorReplicate(1.0f);
	o = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	q = XMQuaternionRotationRollPitchYaw(camera->pitch, camera->yaw, camera->roll);
	t = XMLoadFloat3(&position);
	world = XMMatrixAffineTransformation(s, o, q, t);
	view  = XMMatrixInverse(0, world);

	camera->look = XMVector3Normalize(XMMatrixTranspose(view).r[2]);
	camera->right = XMVector3Normalize(XMMatrixTranspose(view).r[0]);
	camera->up = XMVector3Normalize(XMMatrixTranspose(view).r[1]);


	cb_viewproj.data.view_matrix = view;
}
