#include "stdafx.h"
#include "EffectSystem.h"
#include "TimeMgr.h"

using namespace reality;


void EffectSystem::OnCreate(entt::registry& reg)
{
	
	
}

void EffectSystem::OnUpdate(entt::registry& reg)
{
	auto view_effect = reg.view<C_Effect>();
	for (auto& entity : view_effect)
	{
		auto& effect_comp = reg.get<C_Effect>(entity);
		auto& effect = effect_comp.effect;
		for (auto& pair : effect.emitters)
		{
			auto& emitter = pair.second;
			UpdateParticles(&emitter);

			EmitParticles(&emitter);
		}
	}
	
}

void EffectSystem::UpdateParticles(Emitter* emitter)
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

		// Lifetime에 따른 수정 프레임
		particle.frame_ratio = particle.timer / particle.lifetime;

		// Life Time에 따른 속성 값 조정
		int frame_percentage = particle.frame_ratio * 100.0f;
		frame_percentage = min(100, frame_percentage);

		// COLOR
		switch (emitter->color_setting_type)
		{
		case INITIAL_SET:
			particle.color = emitter->initial_color;
			break;
		case SET_PER_LIFETIME:
			particle.color = emitter->color_timeline[frame_percentage];
			break;
		}

		// SIZE
		switch (emitter->size_setting_type)
		{
		case INITIAL_SET:
			particle.scale = particle.scale;
			break;
		case ADD_PER_LIFETIME:
			XMStoreFloat3(&particle.scale, XMVectorAdd(XMLoadFloat3(&particle.scale), TIMER->GetDeltaTime() * XMLoadFloat3(&particle.add_size)));
			break;
		case SET_PER_LIFETIME:
			particle.scale = emitter->size_timeline[frame_percentage];
			break;
		}

		// 크기는 현재 x값으로만 계산
		particle.scale.x = particle.scale.x;
		particle.scale.y = particle.scale.x;
		particle.scale.z = particle.scale.x;

		// ROTATION
		switch (emitter->rotation_setting_type)
		{
		case INITIAL_SET:
			particle.rotation = particle.rotation;
			break;
		case ADD_PER_LIFETIME:
			particle.rotation.z += TIMER->GetDeltaTime() * particle.add_rotation;
			break;
		case SET_PER_LIFETIME:
			particle.rotation.z = emitter->rotation_timeline[frame_percentage];
			break;
		}

		// POSITION
		switch (emitter->position_setting_type)
		{
		case INITIAL_SET:
			break;
		case ADD_PER_LIFETIME:
			XMVECTOR acc = TIMER->GetDeltaTime() * XMLoadFloat3(&particle.accelation);
			particle.velocity.x += acc.m128_f32[0];
			particle.velocity.y += acc.m128_f32[1];
			particle.velocity.z += acc.m128_f32[2];
			break;
		case SET_PER_LIFETIME:
			particle.velocity = emitter->velocity_timeline[frame_percentage];
			break;
		}

		// GRAVITY
		if (emitter->gravity_on_off)
		{
			XMVECTOR gravity = { 0.0f, -9.8f, 0.0f, 0.0f };
			XMStoreFloat3(&particle.velocity, XMVectorAdd(XMLoadFloat3(&particle.velocity), TIMER->GetDeltaTime() * gravity));
		}
		
		XMStoreFloat3(&particle.position, XMVectorAdd(XMLoadFloat3(&particle.position), TIMER->GetDeltaTime() * XMLoadFloat3(&particle.velocity)));
	}
}

void EffectSystem::EmitParticles(Emitter* emitter)
{
	switch (emitter->emit_type)
	{
	case ONCE:
		// 한 번만 실행되도록
		if (emitter->particles.size() >= emitter->emit_once)
			return;

		for (int i = 0; i < emitter->emit_once; i++)
		{
			EmitParticle(emitter);
		}
		break;
	case PER_SECOND:
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
	case AFTER_TIME:
		if (emitter->timer > emitter->emit_time)
		{
			emitter->timer -= emitter->emit_time;

			for (int i = 0; i < emitter->emit_time; i++)
			{
				EmitParticle(emitter);
			}

		}
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

	particle.color = emitter->initial_color;

	particle.position = {
		randstep(emitter->initial_position[0].x, emitter->initial_position[1].x),
		randstep(emitter->initial_position[0].y, emitter->initial_position[1].y),
		randstep(emitter->initial_position[0].z, emitter->initial_position[1].z) };
	particle.rotation = { 0, 0, randstep(emitter->initial_rotation[0], emitter->initial_rotation[1]) };
	particle.scale = {
		randstep(emitter->initial_size[0].x, emitter->initial_size[1].x),
		randstep(emitter->initial_size[0].y, emitter->initial_size[1].y),
		randstep(emitter->initial_size[0].z, emitter->initial_size[1].z) };

	particle.velocity = {
		randstep(emitter->initial_velocity[0].x, emitter->initial_velocity[1].x),
		randstep(emitter->initial_velocity[0].y, emitter->initial_velocity[1].y),
		randstep(emitter->initial_velocity[0].z, emitter->initial_velocity[1].z) };

	particle.add_size = {
		randstep(emitter->add_size_per_lifetime[0].x, emitter->add_size_per_lifetime[1].x),
		randstep(emitter->add_size_per_lifetime[0].y, emitter->add_size_per_lifetime[1].y),
		randstep(emitter->add_size_per_lifetime[0].z, emitter->add_size_per_lifetime[1].z) };

	particle.add_rotation = randstep(emitter->add_rotation_per_lifetime[0], emitter->add_rotation_per_lifetime[1]);

	particle.accelation = {
		randstep(emitter->accelation_per_lifetime[0].x, emitter->accelation_per_lifetime[1].x),
		randstep(emitter->accelation_per_lifetime[0].y, emitter->accelation_per_lifetime[1].y),
		randstep(emitter->accelation_per_lifetime[0].z, emitter->accelation_per_lifetime[1].z) };
}
