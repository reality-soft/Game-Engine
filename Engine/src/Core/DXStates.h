#pragma once

namespace reality
{
	class DLL_API DXStates
	{
	public:
		static ID3D11BlendState*			bs_default();
		static ID3D11BlendState*			bs_alpha_blending();
		static ID3D11BlendState*			bs_alpha_to_coverage_enable();
		static ID3D11BlendState*			bs_dual_source_blend();
		static ID3D11BlendState*			bs_blend_higher_rgb();


		static ID3D11DepthStencilState*		ds_defalut();
		static ID3D11DepthStencilState*		ds_depth_enable_no_write();
		static ID3D11DepthStencilState*		ds_depth_disable();


		static ID3D11RasterizerState*		rs_solid_cull_none();
		static ID3D11RasterizerState*		rs_solid_cull_back();
		static ID3D11RasterizerState*		rs_solid_cull_front();
		static ID3D11RasterizerState*		rs_wireframe_cull_none();
		static ID3D11RasterizerState*		rs_wireframe_cull_back();
		static ID3D11RasterizerState*		rs_wireframe_cull_front();


		static ID3D11SamplerState*			ss_defalut();
	};
}


