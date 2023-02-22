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
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);

		void SetCbTransform(C_Transform& transform);
		void PlayAnimation(Skeleton& skeleton, C_Animation& animation);
		void SetMaterial(const Material& material);
		void RenderStaticMesh(C_StaticMesh& static_mesh);
		void RenderSkeletalMesh(C_SkeletalMesh& skeletal_mesh); 
		void RenderEffects(entt::registry& reg);
	private:
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* device_context = nullptr;

	private:
		CbTransform cb_transform;
		CbSkeleton cb_skeleton;
	};
}
