#pragma once
#include "RenderTargetMgr.h"
#include "StaticMeshLevel.h"

namespace reality
{
	class DLL_API StaticShadowMap
	{
	public:
		bool Create(XMVECTOR light_position, string vs_id, string ps_id);
		void RenderLevelShadowMap(StaticMeshLevel* static_mesh_level);

	private:
		XMMATRIX shadow_matrix_;

		RenderTarget* shadow_map_rt_ = nullptr;
		VertexShader* vertex_shader_ = nullptr;
		PixelShader* pixel_shader_ = nullptr;

		ComPtr<ID3D11Texture2D> shadow_map_texture_;
		ComPtr<ID3D11ShaderResourceView> shadow_map_srv_;
	};
}

