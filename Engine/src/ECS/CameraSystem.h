#pragma once
#include "System.h"

namespace KGCA41B
{
	class DLL_API CameraSystem : public System
	{
	public:
		CameraSystem();
		~CameraSystem();

		virtual void OnCreate(entt::registry& reg, string tag);
		virtual void OnUpdate(entt::registry& reg);

	private:
		void CameraMovement(InputMapping& input_mapping);
		void CreateMatrix();

	private:
		Camera* camera;
		D3D11_VIEWPORT* viewport;
		XMMATRIX view_matrix, projection_matrix;
	};
}
