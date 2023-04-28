#include "stdafx.h"
#include "CameraSystem.h"
#include "TimeMgr.h"
#include "InputMgr.h"
#include "Engine.h"

using namespace reality;

CameraSystem::CameraSystem()
{
	camera = nullptr;
	viewport = DX11APP->GetViewPortAddress();

	cb_camera_info.data.view_proj_matrix = XMMatrixIdentity();
	projection_matrix = XMMatrixIdentity();
	view_matrix = XMMatrixIdentity();
}

CameraSystem::~CameraSystem()
{
}

void reality::CameraSystem::TargetTag(entt::registry& reg, string tag)
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

		debug_camera_.camera_pos = XMVectorZero();
		debug_camera_.pitch_yaw = { 0 , 0 };
		debug_camera_.near_z = 1.f;
		debug_camera_.far_z = 100000.f;
		debug_camera_.fov = XMConvertToRadians(90);
		debug_camera_.tag = "Debug";

		reg.emplace<C_Camera>(debug_entity_, debug_camera_);
		camera = reg.try_get<C_Camera>(debug_entity_);
	}

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbCameraInfo::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = cb_camera_info.buffer.GetAddressOf();

	HRESULT hr;
	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_camera_info.buffer.GetAddressOf());

	// 빌보드 상수버퍼
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbCameraEffect::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = cb_effect.buffer.GetAddressOf();

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_effect.buffer.GetAddressOf());
}

void CameraSystem::OnUpdate(entt::registry& reg)
{
	auto view_trans = reg.view<C_Transform>();

	if (camera->tag == "Debug")
		DebugCameraMovement();
	else if(camera->tag == "Player")
		PlayerCameraMovement();

	CameraAction();

	CreateMatrix();

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_camera_info.buffer.Get(), 0, nullptr, &cb_camera_info.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_camera_info.buffer.GetAddressOf());

	// 빌보드 상수버퍼 적용
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_effect.buffer.Get(), 0, nullptr, &cb_effect.data, 0, 0);
	DX11APP->GetDeviceContext()->GSSetConstantBuffers(0, 1, cb_effect.buffer.GetAddressOf());
}

void reality::CameraSystem::CreateFrustum()
{
	frustum.CreateFromMatrix(frustum, projection_matrix);
}

RayShape CameraSystem::CreateMouseRay()
{
	RayShape mouse_ray;
	POINT cursor_pos;
	GetCursorPos(&cursor_pos);
	ScreenToClient(ENGINE->GetWindowHandle(), &cursor_pos);

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

	mouse_ray.start = _XMFLOAT3((ray_origin + ray_dir * camera->near_z));
	mouse_ray.end = _XMFLOAT3((ray_origin + ray_dir * camera->far_z));

	return mouse_ray;
}

RayShape reality::CameraSystem::CreateFrontRay()
{
	XMMATRIX camera_world = XMMatrixInverse(nullptr, view_matrix);
	XMVECTOR ray_origin = camera_world.r[3];
	ray_origin.m128_f32[3] = 0.0f;
	XMVECTOR ray_dir = XMVector3Normalize(camera_world.r[2]);

	return RayShape(ray_origin, ray_origin + ray_dir * 100000.0f);
}

C_Camera* CameraSystem::GetCamera()
{
	return camera;
}

XMMATRIX reality::CameraSystem::GetViewProj()  
{
	return view_matrix * projection_matrix;
}

void CameraSystem::DebugCameraMovement()
{
	XMVECTOR front_dir = camera->look *speed* TM_DELTATIME;
	XMVECTOR right_dir = camera->right * speed * TM_DELTATIME;
	XMVECTOR up_dir = camera->up * speed * TM_DELTATIME;

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


void reality::CameraSystem::PlayerCameraMovement()
{
	float yaw   = DINPUT->GetDeltaX() * TM_DELTATIME * mouse_sensivity;
	float pitch = DINPUT->GetDeltaY() * TM_DELTATIME * mouse_sensivity;

	camera->pitch_yaw.x += pitch;
	camera->pitch_yaw.y += yaw;
	camera->roll = 0;
}

void CameraSystem::CameraAction()
{
	// Shaking Action
	if (camera->is_shaking)
	{
		camera->shaking_timer += TIMER->GetDeltaTime();
		if (camera->shaking_timer > camera->shake_time)
		{
			camera->is_shaking = false;
			camera->shaking_timer = 0.0f; 
			camera->shake_time = 0.0f;
			camera->shake_magnitude = 0.0f;
			camera->shake_frequency = 0.0f;
		}
		else
		{
			float shake_displacementX = randstep(-camera->shake_magnitude, camera->shake_magnitude) * sin(TIMER->GetDeltaTime() * camera->shake_frequency);
			float shake_displacementY = randstep(-camera->shake_magnitude, camera->shake_magnitude) * sin(TIMER->GetDeltaTime() * camera->shake_frequency);
			float shake_displacementZ = randstep(-camera->shake_magnitude, camera->shake_magnitude) * sin(TIMER->GetDeltaTime() * camera->shake_frequency);

			//camera->camera_pos = XMVectorAdd(camera->camera_pos, XMVectorSet(shake_displacementX, shake_displacementY, shake_displacementZ, 0.0f));
			camera->roll = shake_displacementX;
			//camera->target_pos = XMVectorAdd(camera->target_pos, XMVectorSet(shake_displacementX, shake_displacementY, shake_displacementZ, 0.0f));
		}
	}
}

void CameraSystem::CreateMatrix()
{
	camera->aspect = viewport->Width / viewport->Height;
	projection_matrix = XMMatrixPerspectiveFovLH(camera->fov, camera->aspect, camera->near_z, camera->far_z);

	XMMATRIX rotation_matrix, view_matrix;
	XMVECTOR scale_vector, rotation_center, rotation_quaternion;

	XMVECTOR up_vector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	camera->camera_pos.m128_f32[3] = 0;

	scale_vector = XMVectorReplicate(1.0f);
	rotation_center = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	rotation_quaternion = DirectX::XMQuaternionRotationRollPitchYaw(camera->pitch_yaw.x, camera->pitch_yaw.y, camera->roll);

	camera->rotation += (camera->target_rotation - camera->rotation) * TM_DELTATIME * 10;

	if (camera->tag == "Debug") {
		XMVECTOR target_pos = camera->camera_pos;
		target_pos.m128_f32[2] += 1;
		view_matrix = XMMatrixLookAtLH(camera->camera_pos, target_pos, up_vector);
		rotation_center = camera->camera_pos;
		rotation_matrix = DirectX::XMMatrixAffineTransformation(scale_vector, XMVectorZero(), rotation_quaternion, rotation_center);
		view_matrix = XMMatrixInverse(0, rotation_matrix);
		camera->camera_pos = rotation_matrix.r[3];
	}
	else {
		rotation_center = camera->target_pos;
		view_matrix = XMMatrixLookAtLH(camera->camera_pos + XMVECTOR{ camera->rotation, 0, camera->rotation, 0 }, camera->target_pos + XMVECTOR{ camera->rotation, 0, camera->rotation, 0 }, up_vector);
		rotation_matrix = DirectX::XMMatrixAffineTransformation(scale_vector, rotation_center, rotation_quaternion, XMVectorZero());
		rotation_matrix = XMMatrixInverse(0, rotation_matrix);
		view_matrix = XMMatrixMultiply(rotation_matrix, view_matrix);
	}

	this->view_matrix = view_matrix;
	this->world_matrix = XMMatrixInverse(0, view_matrix);

	//cb_camera_info.data.view_matrix = XMMatrixTranspose(view_matrix);
	//cb_camera_info.data.projection_matrix = XMMatrixTranspose(projection_matrix);
	cb_camera_info.data.view_proj_matrix = XMMatrixTranspose(view_matrix * projection_matrix);
	cb_camera_info.data.camera_translation = XMMatrixTranspose(XMMatrixTranslationFromVector(cb_camera_info.data.camera_position));
	cb_camera_info.data.camera_position = XMMatrixInverse(nullptr, this->view_matrix).r[3];
	cb_camera_info.data.camera_position.m128_f32[3] = camera->far_z;
	cb_camera_info.data.camera_look = XMVector3Normalize(XMMatrixInverse(nullptr, this->view_matrix).r[2]);

	//camera->look = XMVector3Normalize(rotation_matrix.r[2]);
	camera->look = cb_camera_info.data.camera_look;
	camera->right = XMVector3Normalize(rotation_matrix.r[0]);
	camera->up = XMVector3Normalize(rotation_matrix.r[1]);

	// billboard matrix
	cb_effect.data.view_matrix = XMMatrixTranspose(view_matrix);
	cb_effect.data.projection_matrix = XMMatrixTranspose(projection_matrix);
	XMMATRIX billboard = XMMatrixInverse(0, view_matrix);
	billboard.r[3].m128_f32[0] = 0.0f;
	billboard.r[3].m128_f32[1] = 0.0f;
	billboard.r[3].m128_f32[2] = 0.0f;
	cb_effect.data.main_billboard = XMMatrixTranspose(billboard);

	XMMATRIX x_only = XMMatrixIdentity();
	x_only.r[1].m128_f32[1] = view_matrix.r[1].m128_f32[1];
	x_only.r[1].m128_f32[2] = view_matrix.r[1].m128_f32[2];
	x_only.r[2].m128_f32[1] = view_matrix.r[2].m128_f32[1];
	x_only.r[2].m128_f32[2] = view_matrix.r[2].m128_f32[2];

	billboard = DirectX::XMMatrixInverse(0, x_only);
	billboard.r[3].m128_f32[0] = 0.0f;
	billboard.r[3].m128_f32[1] = 0.0f;
	billboard.r[3].m128_f32[2] = 0.0f;
	cb_effect.data.x_billboard = XMMatrixTranspose(billboard);

	XMMATRIX y_only = XMMatrixIdentity();
	y_only.r[0].m128_f32[0] = view_matrix.r[0].m128_f32[0];
	y_only.r[0].m128_f32[2] = view_matrix.r[0].m128_f32[2];
	y_only.r[2].m128_f32[0] = view_matrix.r[2].m128_f32[0];
	y_only.r[2].m128_f32[2] = view_matrix.r[2].m128_f32[2];

	billboard = DirectX::XMMatrixInverse(0, y_only);
	billboard.r[3].m128_f32[0] = 0.0f;
	billboard.r[3].m128_f32[1] = 0.0f;
	billboard.r[3].m128_f32[2] = 0.0f;
	cb_effect.data.y_billboard = XMMatrixTranspose(billboard);
}