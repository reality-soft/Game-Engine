#include "stdafx.h"
#include "EffectSystem.h"
#include "TimeMgr.h"
#include "SceneMgr.h"

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
		effect_comp.effect_timer += TIMER->GetDeltaTime();

		// if effect's timer is over lifetime, disable effect
		if (effect_comp.effect_lifetime > 0 && effect_comp.effect_timer > effect_comp.effect_lifetime)
		{
			SCENE_MGR->DestroyActor(entity);
			continue;
		}

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
	// Particle Timer Add
	for (auto& particle : emitter->particles)
	{
		// if particle time is over lifetime, disabled
		particle.timer += TIMER->GetDeltaTime();
		if (particle.timer > particle.lifetime)
		{
			particle.enable = false;
			continue;
		}

		// FixedFrame for Lifetime
		particle.frame_ratio = particle.timer / particle.lifetime;

		// Life Time percentage
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

		// ROTATION
		switch (emitter->rotation_setting_type)
		{
		case INITIAL_SET:
			particle.rotation = particle.rotation;
			break;
		case ADD_PER_LIFETIME:
			XMStoreFloat3(&particle.rotation, XMVectorAdd(XMLoadFloat3(&particle.rotation), TIMER->GetDeltaTime() * XMLoadFloat3(&particle.add_rotation)));
			break;
		case SET_PER_LIFETIME:
			particle.rotation = emitter->rotation_timeline[frame_percentage];
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
		
		XMStoreFloat3(&particle.position, XMVectorAdd(XMLoadFloat3(&particle.position), TIMER->GetDeltaTime() * XMLoadFloat3(&particle.velocity)));
	}
}

void EffectSystem::EmitParticles(Emitter* emitter)
{
	emitter->timer += TIMER->GetDeltaTime();

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
			EmitParticle(emitter);
		}
		break;
	}
}

void EffectSystem::EmitParticle(Emitter* emitter)
{
	Particle* new_particle = nullptr;

	for (auto& particle : emitter->particles)
	{
		if (!particle.enable)
		{
			new_particle = &particle;
			break;
		}
	}
	if (new_particle == nullptr)
	{
		emitter->particles.push_back({});
		new_particle = &emitter->particles[emitter->particles.size() - 1];
	}
	

	// 세부 설정
	new_particle->enable = true;

	new_particle->timer = 0.0f;
	new_particle->lifetime = RandomFloatInRange(emitter->life_time[0], emitter->life_time[1]);
	new_particle->frame_ratio = 0.0f;

	switch (emitter->color_setting_type)
	{
	case INITIAL_SET:
		new_particle->color = emitter->initial_color;
		break;
	case SET_PER_LIFETIME:
		new_particle->color = emitter->color_timeline[0];
		break;
	}

	new_particle->position = {
		RandomFloatInRange(emitter->initial_position[0].x, emitter->initial_position[1].x),
		RandomFloatInRange(emitter->initial_position[0].y, emitter->initial_position[1].y),
		RandomFloatInRange(emitter->initial_position[0].z, emitter->initial_position[1].z) };
	new_particle->rotation = {
		RandomFloatInRange(emitter->initial_rotation[0].x, emitter->initial_rotation[1].x),
		RandomFloatInRange(emitter->initial_rotation[0].y, emitter->initial_rotation[1].y),
		RandomFloatInRange(emitter->initial_rotation[0].z, emitter->initial_rotation[1].z) };
	new_particle->scale = {
		RandomFloatInRange(emitter->initial_size[0].x, emitter->initial_size[1].x),
		RandomFloatInRange(emitter->initial_size[0].y, emitter->initial_size[1].y),
		RandomFloatInRange(emitter->initial_size[0].z, emitter->initial_size[1].z) };

	new_particle->velocity = {
		RandomFloatInRange(emitter->initial_velocity[0].x, emitter->initial_velocity[1].x),
		RandomFloatInRange(emitter->initial_velocity[0].y, emitter->initial_velocity[1].y),
		RandomFloatInRange(emitter->initial_velocity[0].z, emitter->initial_velocity[1].z) };

	new_particle->add_size = {
		RandomFloatInRange(emitter->add_size_per_lifetime[0].x, emitter->add_size_per_lifetime[1].x),
		RandomFloatInRange(emitter->add_size_per_lifetime[0].y, emitter->add_size_per_lifetime[1].y),
		RandomFloatInRange(emitter->add_size_per_lifetime[0].z, emitter->add_size_per_lifetime[1].z) };

	new_particle->add_rotation = {
		RandomFloatInRange(emitter->add_rotation_per_lifetime[0].x, emitter->add_rotation_per_lifetime[1].x),
		RandomFloatInRange(emitter->add_rotation_per_lifetime[0].y, emitter->add_rotation_per_lifetime[1].y),
		RandomFloatInRange(emitter->add_rotation_per_lifetime[0].z, emitter->add_rotation_per_lifetime[1].z) };

	new_particle->accelation = {
		RandomFloatInRange(emitter->accelation_per_lifetime[0].x, emitter->accelation_per_lifetime[1].x),
		RandomFloatInRange(emitter->accelation_per_lifetime[0].y, emitter->accelation_per_lifetime[1].y),
		RandomFloatInRange(emitter->accelation_per_lifetime[0].z, emitter->accelation_per_lifetime[1].z) };
}
