#include "stdafx.h"
#include "EffectSystem.h"
#include "TimeMgr.h"

using namespace KGCA41B;


void EffectSystem::OnCreate(entt::registry& reg)
{
	
	
}

void EffectSystem::OnUpdate(entt::registry& reg)
{
	auto view_effect = reg.view<C_Effect>();
	for (auto& entity : view_effect)
	{
		auto& effect = reg.get<C_Effect>(entity);
		for (auto& emitter : effect.emitters)
		{
			UpdateTimer(emitter.get());

			EmitParticles(emitter.get());
		}
	}
	
}

void EffectSystem::UpdateTimer(Emitter* emitter)
{
	emitter->timer += TIMER->GetDeltaTime();

	// 파티클 lifetime 계산
	for (auto& particle : emitter->particles)
	{
		// 파티클 타이머 체크 후 수명이 다한 파티클은 enable
		particle.timer += TIMER->GetDeltaTime();
		if (particle.timer > particle.lifetime)
		{
			particle.enable = false;
			continue;
		}

		particle.frame_ratio = particle.timer / particle.lifetime;

		// TODO : Life Time에 따른 속성 값 조정
		// 알파 값들 조정 
		if (particle.frame_ratio < 0.3f)
		{
			particle.color.w = 1.0f / 0.3f * particle.frame_ratio;
		}
		else if (particle.frame_ratio > 0.3f)
		{
			particle.color.w = 1.0f - 1.0f / 0.7f * (particle.frame_ratio - 0.3f);
		}

		XMVECTOR acc = TIMER->GetDeltaTime() * XMLoadFloat3(&particle.accelation);
		particle.add_velocity.x += acc.m128_f32[0];
		particle.add_velocity.y += acc.m128_f32[1];
		particle.add_velocity.z += acc.m128_f32[2];

		XMStoreFloat3(&particle.scale, XMVectorAdd(XMLoadFloat3(&particle.scale), TIMER->GetDeltaTime() * XMLoadFloat3(&particle.add_size)));
		XMStoreFloat3(&particle.position, XMVectorAdd(XMLoadFloat3(&particle.position), TIMER->GetDeltaTime() * XMLoadFloat3(&particle.add_velocity)));

		// 크기는 현재 x값으로만 계산
		particle.scale.x = particle.scale.x;
		particle.scale.y = particle.scale.x;
		particle.scale.z = particle.scale.x;
		particle.rotation.z += TIMER->GetDeltaTime() * particle.add_rotation;

	}
}

void EffectSystem::EmitParticles(Emitter* emitter)
{
	switch (emitter->emit_type)
	{
	case PER_SECOND:
		// 한 번만 실행되도록
		if (emitter->particles.size() >= emitter->emit_once)
			return;

		for (int i = 0; i < emitter->emit_once; i++)
		{
			EmitParticle(emitter);
		}
		break;
	case ONCE:
		// 1초마다 파티클 생성
		if (emitter->timer > 1.0f)
		{
			emitter->timer -= 1.0f;

			for (int i = 0; i < emitter->emit_per_second; i++)
			{
				EmitParticle(emitter);
			}

		}
		break;
	case PER_FRAME:
		EmitParticle(emitter);
		break;
	}
}

void EffectSystem::EmitParticle(Emitter* emitter)
{
	emitter->particles.push_back({});
	auto& particle = emitter->particles[emitter->particles.size() - 1];

	// 세부 설정
	particle.enable = true;

	particle.timer = 0.0f;
	particle.lifetime = randstep(emitter->life_time[0], emitter->life_time[1]);
	particle.frame_ratio = 0.0f;

	particle.color = emitter->color;

	particle.position = {
		randstep(emitter->initial_position[0].x, emitter->initial_position[1].x),
		randstep(emitter->initial_position[0].y, emitter->initial_position[1].y),
		randstep(emitter->initial_position[0].z, emitter->initial_position[1].z) };
	particle.rotation = { 0, 0, randstep(emitter->initial_rotation[0], emitter->initial_rotation[1]) };
	// 크기는 현재 x값으로만 계산
	particle.scale = {
		randstep(emitter->initial_size[0].x, emitter->initial_size[1].x),
		randstep(emitter->initial_size[0].x, emitter->initial_size[1].x),
		randstep(emitter->initial_size[0].z, emitter->initial_size[1].z) };

	particle.add_velocity = {
		randstep(emitter->initial_velocity[0].x, emitter->initial_velocity[1].x),
		randstep(emitter->initial_velocity[0].y, emitter->initial_velocity[1].y),
		randstep(emitter->initial_velocity[0].z, emitter->initial_velocity[1].z) };

	particle.add_size = {
		randstep(emitter->size_per_lifetime[0].x, emitter->size_per_lifetime[1].x),
		randstep(emitter->size_per_lifetime[0].y, emitter->size_per_lifetime[1].y),
		randstep(emitter->size_per_lifetime[0].z, emitter->size_per_lifetime[1].z) };

	particle.add_rotation = randstep(emitter->rotation_per_lifetime[0], emitter->rotation_per_lifetime[1]);

	particle.accelation = {
		randstep(emitter->accelation_per_lifetime[0].x, emitter->accelation_per_lifetime[1].x),
		randstep(emitter->accelation_per_lifetime[0].y, emitter->accelation_per_lifetime[1].y),
		randstep(emitter->accelation_per_lifetime[0].z, emitter->accelation_per_lifetime[1].z) };
}
