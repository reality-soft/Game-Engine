#pragma once
#include "Components.h"

namespace reality {
	class DLL_API ComponentSystem
	{
		SINGLETON(ComponentSystem)
#define COMPONENT ComponentSystem::GetInst()
	public:
		bool OnInit(entt::registry& registry) {
			registry.on_construct<Component>().connect<&ComponentSystem::OnConstruct<Component>>(this);
			registry.on_construct<C_Transform>().connect<&ComponentSystem::OnConstruct<C_Transform>>(this);
			registry.on_construct<C_StaticMesh>().connect<&ComponentSystem::OnConstruct<C_StaticMesh>>(this);
			registry.on_construct<C_SkeletalMesh>().connect<&ComponentSystem::OnConstruct<C_SkeletalMesh>>(this);
			registry.on_construct<C_Camera>().connect<&ComponentSystem::OnConstruct<C_Camera>>(this);
			registry.on_construct<C_Animation>().connect<&ComponentSystem::OnConstruct<C_Animation>>(this);
			registry.on_update<C_Transform>().connect<&ComponentSystem::OnUpdate<C_Transform>>(this);
			//registry.on_update<C_Animation>().connect<&ComponentSystem::OnUpdate<C_Animation>>(this);

			return true;
		}
		template<typename ComponentType>
		void OnConstruct(entt::registry& registry, entt::entity entity) {
			registry.get<ComponentType>(entity).OnConstruct();
		}

		template <typename ComponentType>
		void OnUpdate(entt::registry& registry, entt::entity entity) {
			registry.get<ComponentType>(entity).OnUpdate();
		}
	};
}

