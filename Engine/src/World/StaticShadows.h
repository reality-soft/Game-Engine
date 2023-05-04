#pragma once
#include "ScreenGrab.h"
#include "RenderTargetMgr.h"
#include "StaticMeshLevel.h"
namespace reality
{
	bool DLL_API CreateDepthBiasRS(int DepthBias, float SlopeScaledDepthBias, float DepthBiasClamp, ID3D11RasterizerState** rs);

	struct CbProjectionShadow
	{
		CbProjectionShadow()
		{
			data.shadow_view_proj = XMMatrixIdentity();
		}
		struct Data
		{
			XMMATRIX shadow_view_proj;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbCubeMapShadow
	{
		CbCubeMapShadow()
		{
			for (int i = 0; i < 6; ++i)
				data.shadow_view_proj[i] = XMMatrixIdentity();
		}
		struct Data
		{
			XMMATRIX shadow_view_proj[6];
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	class DLL_API ProjectionShadow
	{
	public:
		bool Init(XMFLOAT2 near_far, XMFLOAT2 dmap_size, string dmap_vs);
		bool CreateDepthMap(StaticMeshLevel* level_to_render, XMVECTOR light_pos, XMVECTOR light_look);
		void SetDepthMapSRV();
		ID3D11ShaderResourceView* GetDepthMapSRV();

	private:
		XMFLOAT2 projection_near_far_;
		XMFLOAT2 depth_map_size_;

		RenderTarget* depth_map_rt_ = nullptr;
		VertexShader* depth_map_vs_ = nullptr;
		ID3D11RasterizerState* depth_bias_rs_ = nullptr;

		ID3D11SamplerState* sampler_clamp_ = nullptr;
		CbProjectionShadow cb_shadow_;
	};

	class DLL_API CubemapShadow
	{
	public:
		bool Init(XMFLOAT2 near_far, XMFLOAT2 dmap_size, string dmap_vs);
		bool CreateDepthMap(StaticMeshLevel* level_to_render, XMVECTOR light_pos);
		void CreateTextureFile();
		void SetDepthMapSRV(int slot);

		ID3D11ShaderResourceView* depth_srvs[6] = {0,};
		ID3D11RasterizerState* depth_bias_rs_ = nullptr;
	private:
		XMFLOAT2 projection_near_far_;
		XMFLOAT2 depth_map_size_;

		ComPtr<ID3D11RenderTargetView> cubemap_rt_;
		RenderTarget* depth_map_rts_[6];
		VertexShader* depth_map_vs_ = nullptr;

		ID3D11SamplerState* sampler_clamp_ = nullptr;
		CbCubeMapShadow cb_cubemap_shadow_;

		CbProjectionShadow cb_shadow_[6];
	};
}