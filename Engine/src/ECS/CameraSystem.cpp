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

	cb_viewproj.data.view_matrix = XMMatrixIdentity();
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
	auto view = reg.view<C_Camera>();
	for (auto entity : view)
	{
		auto& camera = view.get<C_Camera>(entity);
		if (camera.tag == "Debug")
		{
			this->camera = &camera;
		}
	}

	if (camera == nullptr) {
		entt::entity debug_entity_ = reg.create();
		C_Camera debug_camera_;
		//debug_camera_.camera_matrix = XMMatrixTranslationFromVector({ 0, 0, 0, 0 }) * XMMatrixRotationY(XMConvertToRadians(90));
		debug_camera_.camera_pos = XMVectorZero();
		debug_camera_.pitch_yaw = { 0 , 0 };
		debug_camera_.near_z = 1.f;
		debug_camera_.far_z = 10000.f;
		debug_camera_.fov = XMConvertToRadians(45);
		debug_camera_.tag = "Debug";
		reg.emplace<C_Camera>(debug_entity_, debug_camera_);
		camera = reg.try_get<C_Camera>(debug_entity_);
	}

	//XMVECTOR position = XMVector4Normalize(camera->camera_matrix.r[3]);
	//XMVECTOR target = XMVector4Normalize(camera->world.r[3]);
	//XMVECTOR up = XMVector4Normalize({ 0, 1, 0, 0 });

	//if (camera != nullptr)
		//cb_viewproj.data.view_matrix = XMMatrixLookAtLH(position, target, up);

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
	auto view_trans = reg.view<C_Transform>();

	if (camera->tag == "Debug")
		DebugCameraMovement();
	else
		PlayerCameraMovement();

	CameraAction();

	CreateMatrix();

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_viewproj.buffer.Get(), 0, nullptr, &cb_viewproj.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_viewproj.buffer.GetAddressOf());
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

void CameraSystem::DebugCameraMovement()
{
	XMVECTOR front_dir = look * speed * TM_DELTATIME;
	XMVECTOR right_dir = right * speed * TM_DELTATIME;
	XMVECTOR up_dir = up * speed * TM_DELTATIME;

	if (DINPUT->GetMouseState(R_BUTTON) == KEY_HOLD)
	{
		float yaw = DINPUT->GetDeltaX() * TM_DELTATIME;
		float pitch = DINPUT->GetDeltaY() * TM_DELTATIME;

		camera->pitch_yaw.x += pitch;
		camera->pitch_yaw.y += yaw;
	}

	if (DINPUT->GetKeyState(DIK_W) == KEY_HOLD)
	{
		camera->camera_pos += front_dir;
	}
	if (DINPUT->GetKeyState(DIK_S) == KEY_HOLD)
	{
		camera->camera_pos -= front_dir;
	}
	if (DINPUT->GetKeyState(DIK_A) == KEY_HOLD)
	{
		camera->camera_pos -= right_dir;
	}
	if (DINPUT->GetKeyState(DIK_D) == KEY_HOLD)
	{
		camera->camera_pos += right_dir;
	}
	if (DINPUT->GetKeyState(DIK_Q) == KEY_HOLD)
	{
		camera->camera_pos -= up_dir;
	}
	if (DINPUT->GetKeyState(DIK_E) == KEY_HOLD)
	{
		camera->camera_pos += up_dir;
	}
}

void KGCA41B::CameraSystem::PlayerCameraMovement()
{
	if (DINPUT->GetMouseState(R_BUTTON) == KEY_HOLD)
	{
		float yaw = DINPUT->GetDeltaX() * TM_DELTATIME;
		float pitch = DINPUT->GetDeltaY() * TM_DELTATIME;

		camera->pitch_yaw.x += pitch;
		camera->pitch_yaw.y += yaw;
	}

	camera->OnUpdate();
}

void KGCA41B::CameraSystem::UpdateVectors()
{
	look = XMVector3Normalize(world_matrix.r[2]);
	right = XMVector3Normalize(world_matrix.r[0]);
	up = XMVector3Normalize(world_matrix.r[1]);
}

void CameraSystem::CameraAction()
{

}

void CameraSystem::CreateMatrix()
{
	camera->aspect = viewport->Width / viewport->Height;
	projection_matrix = XMMatrixPerspectiveFovLH(camera->fov, camera->aspect, camera->near_z, camera->far_z);

	XMMATRIX w, v;
	XMVECTOR S, O, Q, T;

	camera->camera_pos.m128_f32[3] = 0;

	S = XMVectorReplicate(1.0f);
	O = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	Q = DirectX::XMQuaternionRotationRollPitchYaw(camera->pitch_yaw.x, camera->pitch_yaw.y, 0);
	if (camera->tag == "Debug") {
		w = DirectX::XMMatrixAffineTransformation(S, O, Q, camera->camera_pos);
	}
	else {
		w = DirectX::XMMatrixAffineTransformation(S, -camera->local_pos * Q, Q, camera->camera_pos);
	}
	v = DirectX::XMMatrixInverse(0, w);

	view_matrix = v;
	camera->camera_pos = w.r[3];

	cb_viewproj.data.view_matrix = XMMatrixTranspose(view_matrix);
	cb_viewproj.data.projection_matrix = XMMatrixTranspose(projection_matrix);
	cb_viewproj.data.camera_position = camera->camera_pos;
	cb_viewproj.data.camera_position.m128_f32[3] = camera->far_z;

	look = XMVector3Normalize(w.r[2]);
	right = XMVector3Normalize(w.r[0]);
	up = XMVector3Normalize(w.r[1]);
}