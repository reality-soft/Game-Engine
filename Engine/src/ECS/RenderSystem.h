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

		void SetCbTransform(const C_Transform* const transform);
		void PlayAnimation(const Skeleton& skeleton, const vector<OutAnimData>& res_animation);
		void RenderStaticMesh(const C_StaticMesh* const static_mesh);
		void RenderSkeletalMesh(const C_SkeletalMesh* const skeletal_mesh_components, const C_Animation* const animation_component);
		

		// Effect Rendering
		void CreateEffectCB();
		void CreateEffectBuffer();

		void RenderBoxShape(entt::registry& reg);
		void RenderBoundingBox(const C_BoundingBox* const);
		void RenderEffects(entt::registry& reg);
			void SetEffectCB(Effect& effect, XMMATRIX& world);
			void SetEmitterCB(Emitter& emitter);
			void SetShaderAndMaterial(Emitter& emitter);
			void SetStates(Emitter& emitter);
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
		CbEffect	cb_effect;
		CbEmitter	cb_emitter;
		CbParticle	cb_particle_;
	};
}
