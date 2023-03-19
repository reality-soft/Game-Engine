#pragma once
#include "UIBase.h"
#include "RenderTargetMgr.h"
namespace reality
{
	class DLL_API UI_Minimap : public UIBase
	{
	private:
		string minimap_tex_id_;
		entt::registry* reg_;
		shared_ptr<RenderTarget> minimap_rt;
	public:
		void InitMinimap(string minimap_id);
	protected:
		void UpdateThisUI() override;
		void RenderThisUI() override;
	private:
		void RenderMinimap();
		void RenderPlayerIcon(XMMATRIX world);
		void RenderZombieIcon(XMMATRIX world);
		void RenderExceptTextureSet();
	private:
		XMVECTOR ConvertWorldToScreenXZ(XMVECTOR world);
	};
}


