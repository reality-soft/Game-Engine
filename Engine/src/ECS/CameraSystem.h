#pragma once
#include "System.h"

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

		MouseRay CreateMouseRay();
		C_Camera* GetCamera();
		XMMATRIX GetViewProj();
		void SetSpeed(float speed) { this->speed = speed; };
		XMFLOAT2 ndc;
	private:
		void DebugCameraMovement();
		void PlayerCameraMovement();

		void UpdateVectors();
		void CameraAction();
		void CreateMatrix();

	private:
		C_Camera* camera = nullptr;
		D3D11_VIEWPORT* viewport;

		XMMATRIX world_matrix;
		XMMATRIX view_matrix;
		XMMATRIX projection_matrix;

		XMVECTOR look;
		XMVECTOR right;
		XMVECTOR up;

		CbViewProj		cb_viewproj;
		CbCameraEffect	cb_effect;

		float speed = 30;
	};
}
