#pragma once
#include "UIBase.h"
#include "RenderTargetMgr.h"
namespace reality
{
	class UI_Minimap : public UIBase
	{
	private:
		string minimap_tex_id_;
		entt::registry* reg_;
		shared_ptr<RenderTarget> minimap_rt;
	private:
		vector<UIVertex>		minimap_vertex_list_;
		ComPtr<ID3D11Buffer>	minimap_vertex_buf_;
		XMFLOAT2				minimap_player_pos;
	public:
		void InitMinimap(string minimap_id);
	protected:
		void UpdateThisUI() override;
		void RenderThisUI() override;
	private:
		void MakeMinimapVB();
		void RenderMinimap();
		void RenderPlayerIcon(XMMATRIX world);
		void RenderZombieIcon(XMMATRIX world);
		void UpdateTextureCoordAlongPlayer();
		void RenderExceptTextureSet();
	private:
		XMVECTOR ConvertWorldToScreenXZ(XMVECTOR world);
	};
}


