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
		Camera* GetCamera();
	private:
		void CameraMovement(InputMapping& input_mapping);
		void CameraAction(InputMapping& input_mapping);
		void CreateMatrix();

	private:
		Camera* camera;
		D3D11_VIEWPORT* viewport;

		XMMATRIX view_matrix;
		XMMATRIX projection_matrix;

		CbViewProj cb_viewproj;
	};
}
