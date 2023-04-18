#pragma once
#include "RenderTargetMgr.h"
#include "StaticMeshLevel.h"

namespace reality
{
	struct CbSingleShadow
	{
		CbSingleShadow()
		{
			data.shadow_view_proj = XMMatrixIdentity();
		}
		struct Data
		{
			XMMATRIX shadow_view_proj;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbMultipleShadows
	{
		CbMultipleShadows()
		{
			for (int i = 0; i < 9; ++i)
			{
				data.shadow_view_proj[i] = XMMatrixIdentity();
			}
		}
		struct Data
		{
			XMMATRIX shadow_view_proj[9];
		} data;

		UINT index = 0;
		ComPtr<ID3D11Buffer> buffer;
	};

	class DLL_API SingleShadow
	{
	public:
		bool Init(XMFLOAT2 near_far, XMFLOAT2 dmap_size, XMFLOAT2 smap_size, string dmap_vs, string smap_vs, string smap_ps);
		void RenderDepthMap(XMVECTOR light_pos, XMVECTOR light_look);
		void RenderShadowMap();
		void SetShadowMapSRV();

		ID3D11ShaderResourceView* GetDepthMapSRV();
		ID3D11ShaderResourceView* GetRTSRV();

	public:
		XMFLOAT2 projection_near_far_;
		XMFLOAT2 depth_map_size_;
		XMFLOAT2 shadow_map_size_;
		StaticMeshLevel* static_mesh_level_ = nullptr;
		ID3D11RasterizerState* depth_bias_rs_;

	private:
		RenderTarget* depth_map_rt_ = nullptr;
		RenderTarget* shadow_map_rt_ = nullptr;
		VertexShader* depth_map_vs_ = nullptr;
		VertexShader* shadow_map_vs_ = nullptr;
		PixelShader* shadow_map_ps_ = nullptr;

		ID3D11SamplerState* sampler_clamp_ = nullptr;
		CbSingleShadow cb_single_shadow_;
		bool depth_map_rendered_ = false;
	};


	class DLL_API MultipleShadows
	{
	public:
		bool Init(XMFLOAT2 near_far, XMFLOAT2 dmap_size, XMFLOAT2 smap_size, string dmap_vs, string smap_vs, string smap_ps);
		bool CreateDepthMap(XMVECTOR light_pos, XMVECTOR light_dir);
		void RenderShadowMap();


		XMFLOAT2 projection_near_far_;
		XMFLOAT2 depth_map_size_;
		XMFLOAT2 shadow_map_size_;
		StaticMeshLevel* static_mesh_level_ = nullptr;

	private:
		RenderTarget* depth_map_rt_ = nullptr;
		RenderTarget* shadow_map_rt_ = nullptr;
		VertexShader* depth_map_vs_ = nullptr;
		VertexShader* shadow_map_vs_ = nullptr;
		PixelShader* shadow_map_ps_ = nullptr;

		ID3D11SamplerState* sampler_clamp_ = nullptr;
		ID3D11RasterizerState* depth_bias_rs_;
		vector<ID3D11ShaderResourceView*> depth_map_textures_;

		CbMultipleShadows cb_total_shadows_;
	};
}