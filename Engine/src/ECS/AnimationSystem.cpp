#include "stdafx.h"
#include "AnimationSystem.h"
#include "TimeMgr.h"
#include "ResourceMgr.h"

using namespace KGCA41B;

AnimationSystem::AnimationSystem()
{
	device_context = DX11APP->GetDeviceContext();
}

AnimationSystem::~AnimationSystem()
{
	device_context = nullptr;
}

void AnimationSystem::OnCreate(entt::registry& reg)
{

}

void AnimationSystem::OnUpdate(entt::registry& reg)
{
	//auto view_anim = reg.view<Skeleton, Animation>();
	//for (auto ent : view_anim)
	//{
	//	auto [skeleton, animation] = view_anim.get<Skeleton, Animation>(ent);
	//	PlayAnimation(skeleton, animation);
	//}
}

void AnimationSystem::PlayAnimation(Skeleton& skeleton, Animation& animation)
{
	//map<UINT, XMMATRIX>* res_skeleton = RESOURCE->UseResource<map<UINT, XMMATRIX>>(skeleton.skeleton_id);
	//vector<OutAnimData>* res_animation = RESOURCE->UseResource<vector<OutAnimData>>(animation.anim_id);

	//static float keyframe = res_animation->begin()->start_frame;
	//
	//if (keyframe >= res_animation->begin()->end_frame)
	//	keyframe = res_animation->begin()->start_frame;

	//for (auto bp : *res_skeleton)
	//{
	//	XMMATRIX anim_matrix = bp.second * res_animation->begin()->animations.find(bp.first)->second[keyframe];
	//	skeleton.cb_skeleton.mat_skeleton[bp.first] = XMMatrixTranspose(anim_matrix);
	//}

	//keyframe += 60.f / TM_FPS;
}
