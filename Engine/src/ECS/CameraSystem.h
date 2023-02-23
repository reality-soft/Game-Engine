#pragma once
#include "System.h"

namespace KGCA41B
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
		XMFLOAT2 ndc;
	private:
		void CameraMovement(C_InputMapping& input_mapping);
		void CameraAction(C_InputMapping& input_mapping);
		void CreateMatrix();

	private:
		C_Camera* camera;
		D3D11_VIEWPORT* viewport;

		XMMATRIX world_matrix;
		XMMATRIX view_matrix;
		XMMATRIX projection_matrix;

		CbViewProj cb_viewproj;
	};
}
