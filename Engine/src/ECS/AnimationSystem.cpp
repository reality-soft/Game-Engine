#include "AnimationSystem.h"
#include "TimeMgr.h"

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
	auto view_anim = reg.view<Skeleton, Animation>();
	for (auto ent : view_anim)
	{
		auto [skeleton, animation] = view_anim.get<Skeleton, Animation>(ent);
		PlayAnimation(skeleton, animation);
	}
}

void AnimationSystem::PlayAnimation(Skeleton& skeleton, Animation& animation)
{
	static float keyframe = animation.start_frame;
	
	if (keyframe >= animation.end_frame)
		keyframe = animation.start_frame;

	for (auto bp : skeleton.bind_poses)
	{
		XMMATRIX anim_matrix = bp.second * animation.anim_track.find(bp.first)->second[keyframe];
		skeleton.cb_skeleton.mat_skeleton[bp.first] = XMMatrixTranspose(anim_matrix);
	}

	keyframe += 60.f / TM_FPS;
}
