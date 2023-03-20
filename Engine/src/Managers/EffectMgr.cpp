#include "stdafx.h"
#include "EffectMgr.h"
#include "SceneMgr.h"

using namespace reality;

template<typename EffectActorClass>
void EffectMgr::SpawnEffect(XMVECTOR pos, XMVECTOR rotation_q)
{
	auto entity = SCENE_MGR->AddActor<EffectActorClass>();
	auto effect_actor = SCENE_MGR->GetActor<EffectActorClass>(entity);
	effect_actor->Spawn(pos, rotation_q);
}
