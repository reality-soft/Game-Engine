#pragma once
#include "Components.h"
#include "SceneMgr.h"

namespace reality
{
	class DLL_API EffectMgr
	{
		SINGLETON(EffectMgr)
#define EFFECT_MGR EffectMgr::GetInst()
	private:
		template<typename EffectActorClass>
		EffectActorClass* MakeEffectActor()
		{
			auto entity = SCENE_MGR->AddActor<EffectActorClass>();
			return SCENE_MGR->GetActor<EffectActorClass>(entity);
		}
		template<typename EffectActorClass>
		EffectActorClass* MakeEffectActor(E_SceneType type)
		{
			auto entity = SCENE_MGR->GetScene(type)->AddActor<EffectActorClass>();
			return SCENE_MGR->GetScene(type)->GetActor<EffectActorClass>(entity);
		}
	
	public:
		// Effect
		template<typename EffectActorClass>
		void SpawnEffect(XMVECTOR pos, XMVECTOR rot_q = XMQuaternionIdentity(), XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f))
		{
			EffectActorClass* effect_actor = MakeEffectActor<EffectActorClass>();
			effect_actor->Spawn(pos, rot_q, scale);
		}
		template<typename EffectActorClass>
		void SpawnEffectFromNormal(XMVECTOR pos, XMVECTOR normal, XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f))
		{
			EffectActorClass* effect_actor = MakeEffectActor<EffectActorClass>();

			XMVECTOR base = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			XMVECTOR axis = XMVector3Cross(base, normal);
			float angle = acosf(XMVectorGetX(XMVector3Dot(base, normal)));
			XMVECTOR rot_q = XMQuaternionRotationAxis(axis, angle);

			effect_actor->Spawn(pos, rot_q, scale);
		}
		template<typename EffectActorClass>
		void SpawnEffect(E_SceneType type, XMVECTOR pos, XMVECTOR rot_q = XMQuaternionIdentity(), XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f))
		{
			EffectActorClass* effect_actor = MakeEffectActor<EffectActorClass>(type);
			effect_actor->Spawn(pos, rot_q, scale);
		}
		template<typename EffectActorClass>
		void SpawnEffectFromNormal(E_SceneType type, XMVECTOR pos, XMVECTOR normal, XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f))
		{
			EffectActorClass* effect_actor = MakeEffectActor<EffectActorClass>(type);

			XMVECTOR base = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			XMVECTOR axis = XMVector3Cross(base, normal);
			float angle = acosf(XMVectorGetX(XMVector3Dot(base, normal)));
			XMVECTOR rot_q = XMQuaternionRotationAxis(axis, angle);

			effect_actor->Spawn(pos, rot_q, scale);
		}
	};
}
