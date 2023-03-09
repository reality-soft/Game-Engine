#pragma once
#include "entt.hpp"
#include "stdafx.h"
#include "DllMacro.h"
#include "TimeMgr.h"
#include "InputMgr.h"
#include "../ECS/Actors/Actor.h"

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
	protected:
		entt::registry reg_scene_;
	};
}
