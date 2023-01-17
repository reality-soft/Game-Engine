#pragma once
#include "DllMacro.h"

namespace KGCA41B {
	class DLL_API Engine
	{
	public:
		Engine();
		virtual ~Engine();
		void Run();
	};

	Engine* CreateEngine();
}


