#pragma once
#include "stdafx.h"
#include "DllMacro.h"
#include "TimeMgr.h"
#include "InputMgr.h"

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
	};
}
