#pragma once
#include "Components.h"

namespace reality
{
	class DLL_API EffectMgr
	{
		SINGLETON(EffectMgr)
#define EFFECT_MGR EffectMgr::GetInst()
		
		template<typename EffectActorClass>
		void SpawnEffect(XMVECTOR pos, XMVECTOR rotation_q);
	};
}
