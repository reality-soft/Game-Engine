#pragma once
#include "System.h"
#include "FbxOutData.h"

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

		void SetCbTransform(const C_Transform& transform);
		void PlayAnimation(const Skeleton& skeleton, const vector<OutAnimData>& res_animation);
		void RenderStaticMesh(C_StaticMesh& static_mesh);
		void RenderSkeletalMesh(const C_SkeletalMesh& skeletal_mesh, const C_Animation& animation_component);
		void RenderBoxShape(BoxShape& box_shape);
		void SetParticle(Particle& particle);
		void SetSprite(Sprite* sprite);
		void RenderParticle(Particle& particle);
	private:
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* device_context = nullptr;

	private:
		CbTransform cb_transform;
		CbSkeleton cb_skeleton;

		// Effect
		CbParticle cb_particle;
		CbSprite cb_sprite;
	};
}
