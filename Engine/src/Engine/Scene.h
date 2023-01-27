#pragma once
#include "FbxLoader.h"

namespace KGCA41B
{
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
