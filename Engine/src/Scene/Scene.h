#pragma once
#include "TimeMgr.h"
#include "InputMgr.h"
#include "Actor.h"

namespace reality {
	class DLL_API Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;
		virtual void OnRelease() = 0;
	public:
		entt::registry& GetRegistryRef() {
			return reg_scene_;
		}
	protected:
		entt::registry reg_scene_;
	};
}
