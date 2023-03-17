#pragma once
#include "System.h"
#include "RenderTargetMgr.h"

namespace reality
{
	class DLL_API UISystem : public System
	{
	public:
		static CbUI	cb_UI;
	public:
		static void SetCbData(XMMATRIX world);
	public:
		shared_ptr<RenderTarget> render_target_;
	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
	};
}

