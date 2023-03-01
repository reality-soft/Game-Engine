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
		

		// Effect Rendering
		void CreateEffectCB();
		void CreateEffectBuffer();

		void RenderBoxShape(entt::registry& reg);

		void RenderEffects(entt::registry& reg);
			void SetEffectCB(entt::registry& reg, C_Effect& effect);
			void SetSpriteCB(Sprite* sprite);
			void SetShaderAndMaterial(Emitter* emitter);
			void SetStates(Emitter* emitter);
			void SetParticleCB(Particle& particle);
	public:

	private:
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* device_context = nullptr;

	private:
		CbTransform cb_transform;
		CbSkeleton cb_skeleton;

		// Effect
		EffectVertex			effect_vertex_;
		ComPtr<ID3D11Buffer>	vertex_buffer_;
		CbEffect	cb_effect_;
		CbParticle	cb_particle_;
		CbSprite	cb_sprite_;
	};
}
