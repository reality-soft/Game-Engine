#pragma once
#include "System.h"
#include "FbxOutData.h"
#include "ConstantBuffer.h"
#include "Mesh.h"
#include "Effect.h"

namespace reality
{
	class DLL_API RenderSystem : public System
	{
	public:
		RenderSystem();
		~RenderSystem();

	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);

		void PlayAnimation(const Skeleton& skeleton, const C_Animation* const animation_component);
		void RenderStaticMesh(const C_StaticMesh* const static_mesh);
		void RenderSkeletalMesh(const C_SkeletalMesh* const skeletal_mesh_components, C_Animation* const animation_component);
		
		// Effect Rendering
		void CreateEffectCB();
		void CreateEffectBuffer();

		void RenderBoxShape(entt::registry& reg);
		void RenderEffects(entt::registry& reg);

	public:
		void SetEffectCB(Effect& effect, XMMATRIX& world);
		void SetEmitterCB(Emitter& emitter);
		void SetShaderAndMaterial(Emitter& emitter);
		void SetStates(Emitter& emitter);
		void SetParticleCB(Particle& particle);

	public:

	private:
		ID3D11Device* device_ = nullptr;
		ID3D11DeviceContext* device_context_ = nullptr;

	private:
		CbTransform cb_transform_;
		CbStaticMesh cb_static_mesh_;
		CbSkeletalMesh cb_skeletal_mesh_;

		// Effect
		EffectVertex			effect_vertex_;
		ComPtr<ID3D11Buffer>	vertex_buffer_;
		CbEffect	cb_effect_;
		CbEmitter	cb_emitter_;
		CbParticle	cb_particle_;
	};
}
