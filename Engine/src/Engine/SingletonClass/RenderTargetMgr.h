#pragma once
#include "DllMacro.h"
#include "Components.h"

namespace KGCA41B
{
	class DLL_API RenderTarget
	{
	public:
		float								width_;
		float								height_;
	public:
		ComPtr<ID3D11RenderTargetView>		render_target_view_;
		ComPtr<ID3D11DepthStencilView>		depth_stencil_view_;
	public:
		ComPtr<ID3D11Texture2D>				render_target_view_texture_;
		ComPtr<ID3D11Texture2D>				depth_stencil_view_texture_;
		ComPtr<ID3D11ShaderResourceView>	render_target_view_srv_;
		ComPtr<ID3D11ShaderResourceView>	depth_stencil_view_srv_;
	public:
		D3D11_TEXTURE2D_DESC				texture_desc_;
		D3D11_DEPTH_STENCIL_VIEW_DESC		dsv_desc;
		D3D11_SHADER_RESOURCE_VIEW_DESC		srv_desc_;
	public:
		D3D11_VIEWPORT						view_port_;
	public:
		// Old RenderTarget Info
		D3D11_VIEWPORT						old_view_port_;
		UINT								view_ports_count_;
		ID3D11RenderTargetView*				old_rtv_;
		ID3D11DepthStencilView*				old_dsv_;
	public:
		bool		Init()  { return false; };
		bool		Frame()  { return false; };
		bool		Render() { return false; };
		bool		Release() { return false; };
	public:
		bool		Create(float rtWidth, float rtHeight);
		void		SetViewPort();
		HRESULT		CreateRenderTargetView();
		HRESULT		CreateDepthStencilView();
	public:
		bool		SetRenderTarget();
	};

	class DLL_API RenderTargetMgr
	{
		SINGLETON(RenderTargetMgr)
#define RENDER_TARGET RenderTargetMgr::GetInst()
	private:
		map<string, shared_ptr<RenderTarget>> resdic_render_target_;
	public:
		void Init(string back_buffer_name);
	public:
		shared_ptr<RenderTarget> MakeRT(std::string rtname, float rtWidth, float rtHeight);
		shared_ptr<RenderTarget> LoadRT(std::string rtname);
	public:
		void	DeletingRT();
		void	ResettingRT();
	};
}
