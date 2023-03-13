#include "stdafx.h"
#include "RenderTargetMgr.h"

using namespace reality;

bool RenderTarget::Create(float width, float height)
{
	HRESULT hr;

	width_ = width;
	height_ = height;

	SetViewPort();

	if (FAILED(hr = CreateRenderTargetView()))
		return hr;

	if (FAILED(hr = CreateDepthStencilView()))
		return hr;

	clear_color_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	return false;
}

void RenderTarget::SetViewPort()
{
	view_port_.Width = width_;
	view_port_.Height = height_;
	view_port_.TopLeftX = 0;
	view_port_.TopLeftY = 0;
	view_port_.MinDepth = 0.0f;
	view_port_.MaxDepth = 1.0f;
}

void reality::RenderTarget::SetClearColor(XMFLOAT4 color)
{
	clear_color_ = color;
}

HRESULT RenderTarget::CreateRenderTargetView()
{
	HRESULT hr;

	ZeroMemory(&texture_desc_, sizeof(texture_desc_));

	texture_desc_.Width = (UINT)width_;
	texture_desc_.Height = (UINT)height_;
	texture_desc_.MipLevels = 1;
	texture_desc_.ArraySize = 1;
	texture_desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc_.SampleDesc.Count = 1;
	texture_desc_.SampleDesc.Quality = 0;
	texture_desc_.Usage = D3D11_USAGE_DEFAULT;
	texture_desc_.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture_desc_.CPUAccessFlags = 0;
	texture_desc_.MiscFlags = 0;

	if (FAILED(hr = DX11APP->GetDevice()->CreateTexture2D(&texture_desc_, NULL, render_target_view_texture_.GetAddressOf())))
		return hr;
	if (FAILED(hr = DX11APP->GetDevice()->CreateShaderResourceView(render_target_view_texture_.Get(), NULL, render_target_view_srv_.GetAddressOf())))
		return hr;
	if (FAILED(hr = DX11APP->GetDevice()->CreateRenderTargetView(render_target_view_texture_.Get(), NULL, &render_target_view_)))
		return hr;

	return hr;
}

HRESULT RenderTarget::CreateDepthStencilView()
{
	HRESULT hr;
	texture_desc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture_desc_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(hr = DX11APP->GetDevice()->CreateTexture2D(&texture_desc_, NULL, depth_stencil_view_texture_.GetAddressOf())))
	{
		return hr;
	}

	ZeroMemory(&dsv_desc, sizeof(dsv_desc));

	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (FAILED(hr = DX11APP->GetDevice()->CreateDepthStencilView(depth_stencil_view_texture_.Get(), &dsv_desc, &depth_stencil_view_)))
		return hr;

	ZeroMemory(&srv_desc_, sizeof(srv_desc_));

	srv_desc_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc_.Texture2D.MipLevels = 1;
	srv_desc_.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	if (FAILED(hr = DX11APP->GetDevice()->CreateShaderResourceView(depth_stencil_view_texture_.Get(), &srv_desc_, depth_stencil_view_srv_.GetAddressOf())))
		return hr;

	return hr;
}

bool RenderTarget::SetRenderTarget()
{
	UINT numViewPorts = 1;

	ID3D11RenderTargetView* pNullRTV = NULL;
	ID3D11DepthStencilView* pNullDSV = NULL;
	DX11APP->GetDeviceContext()->OMSetRenderTargets(1, &pNullRTV, pNullDSV);
	DX11APP->GetDeviceContext()->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);
	DX11APP->GetDeviceContext()->ClearRenderTargetView(render_target_view_, (FLOAT*)&clear_color_);
	DX11APP->GetDeviceContext()->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DX11APP->GetDeviceContext()->RSSetViewports(1, &view_port_);

	return false;
}


void RenderTargetMgr::Init()
{

}

shared_ptr<RenderTarget> RenderTargetMgr::MakeRT(std::string rtname, float rtWidth, float rtHeight)
{
	if (resdic_render_target_.find(rtname) != resdic_render_target_.end())
		return LoadRT(rtname);

	shared_ptr<RenderTarget> newRT = make_shared<RenderTarget>();
	newRT->Create(rtWidth, rtHeight);
	resdic_render_target_.insert({ rtname, newRT });

	return newRT;
}

shared_ptr<RenderTarget> RenderTargetMgr::LoadRT(std::string rtname)
{
	auto iter = resdic_render_target_.find(rtname);
	if (iter != resdic_render_target_.end())
	{
		return iter->second;
	}
	else
	{
		return nullptr;
	}
}

void RenderTargetMgr::DeletingRT()
{
}

void RenderTargetMgr::ResettingRT()
{
}
