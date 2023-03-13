#pragma once
#include "System.h"
#include "RenderTargetMgr.h"

namespace reality
{
	class DLL_API UISystem : public System
	{
	public:
		shared_ptr<RenderTarget> render_target_;
		CbUI	cb_UI;
	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
	private:
		void SetCbData();
	};
}

