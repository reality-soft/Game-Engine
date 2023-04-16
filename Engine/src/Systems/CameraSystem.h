#pragma once
#include "System.h"
#include "ConstantBuffer.h"

namespace reality
{
	class DLL_API CameraSystem : public System
	{
	public:
		CameraSystem();
		~CameraSystem();

		void TargetTag(entt::registry& reg, string tag);
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);


		RayShape CreateMouseRay();
		RayShape CreateFrontRay();
		C_Camera* GetCamera();
		XMMATRIX GetViewProj();
		void SetSpeed(float speed) { this->speed = speed; };
		void SetFov(float dgree) { camera->fov = XMConvertToRadians(dgree); };
		void SetFarZ(float z) { camera->far_z = z; };
		XMFLOAT2 ndc;

		void CreateFrustum();
		BoundingFrustum frustum;
		XMMATRIX world_matrix;
		XMMATRIX view_matrix;
		XMMATRIX projection_matrix;
		float mouse_sensivity = 0.5f;

	private:
		void DebugCameraMovement();
		void PlayerCameraMovement();

		void CameraAction();
		void CreateMatrix();

	private:
		C_Camera* camera = nullptr;
		D3D11_VIEWPORT* viewport;

		CbCameraInfo	cb_camera_info;
		CbCameraEffect	cb_effect;

		float speed = 30;
	};
}
