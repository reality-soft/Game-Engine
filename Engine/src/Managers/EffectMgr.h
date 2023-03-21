#pragma once
#include "Components.h"
#include "SceneMgr.h"

namespace reality
{
	class DLL_API EffectMgr
	{
		SINGLETON(EffectMgr)
#define EFFECT_MGR EffectMgr::GetInst()
	public:
		template<typename EffectActorClass>
		void SpawnEffect(XMVECTOR pos, XMVECTOR rotation_q)
		{
			auto entity = SCENE_MGR->AddActor<EffectActorClass>();
			auto effect_actor = SCENE_MGR->GetActor<EffectActorClass>(entity);
			effect_actor->Spawn(pos, rotation_q);
		}
	};
}
