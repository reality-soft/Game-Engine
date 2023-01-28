#pragma once
#include "System.h"

namespace KGCA41B
{
	class DLL_API RenderSystem : public System
	{
	public:
		RenderSystem();
		~RenderSystem();

	public:
		virtual void OnCreate(entt::registry reg);
		virtual void OnUpdate(entt::registry reg);

		void SetCbTransform(Transform& transform);
		void SetCbSkeleton(Skeleton& skeleton);
		void SetMaterial(Material& material);
		void RenderStaticMesh(StaticMesh& static_mesh);
		void RenderSkeletalMesh(SkeletalMesh& skeletal_mesh);

	private:
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* device_context = nullptr;
	};
}
