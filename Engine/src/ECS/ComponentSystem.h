#pragma once
#include "entt.hpp"
#include "../Engine/common.h"
#include "../Engine/DLLMacro.h"
#include "Components.h"

namespace KGCA41B {
	class DLL_API ComponentSystem
	{
		SINGLETON(ComponentSystem)
	public:
		bool OnInit(entt::registry& registry) {
			registry.on_construct<Component>().connect<&ComponentSystem::OnConstruct<Component>>(this);
			registry.on_construct<Transform>().connect<&ComponentSystem::OnConstruct<Transform>>(this);
			registry.on_construct<StaticMesh>().connect<&ComponentSystem::OnConstruct<StaticMesh>>(this);
			registry.on_construct<SkeletalMesh>().connect<&ComponentSystem::OnConstruct<SkeletalMesh>>(this);
			registry.on_construct<Material>().connect<&ComponentSystem::OnConstruct<Material>>(this);
			registry.on_construct<Camera>().connect<&ComponentSystem::OnConstruct<Camera>>(this);
			registry.on_construct<Skeleton>().connect<&ComponentSystem::OnConstruct<Skeleton>>(this);
			registry.on_construct<Animation>().connect<&ComponentSystem::OnConstruct<Animation>>(this);
			registry.on_construct<InputMapping>().connect<&ComponentSystem::OnConstruct<InputMapping>>(this);

			return true;
		}
		template<typename ComponentType>
		void OnConstruct(entt::registry& registry, entt::entity entity) {
			registry.get<ComponentType>(entity).OnConstruct();
		}
	};
}

