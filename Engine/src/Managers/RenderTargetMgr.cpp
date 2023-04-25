#include "stdafx.h"
#include "RenderTargetMgr.h"
#include "ResourceMgr.h"
#include "Engine.h"

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

	CreateRenderData();

	return false;
}

void RenderTarget::CreateRenderData() 
{
	rect_.SetRectByCenter({ ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y / 2.0f }, ENGINE->GetWindowSize().x, ENGINE->GetWindowSize().y);

	// 쉐이더 이름 입력
	render_data_.vs_id = "UIVS.cso";
	render_data_.ps_id = "UIPS.cso";

	// 정점 작성
	render_data_.vertex_list.push_back({ { -1.0f, +1.0f }, {1.0f, 1.0f, 1.0f, 1.0f} ,{+0.0f, +0.0f} });
	render_data_.vertex_list.push_back({ { +1.0f, +1.0f }, {1.0f, 1.0f, 1.0f, 1.0f} ,{+1.0f, +0.0f} });
	render_data_.vertex_list.push_back({ { -1.0f, -1.0f }, {1.0f, 1.0f, 1.0f, 1.0f} ,{+0.0f, +1.0f} });
	render_data_.vertex_list.push_back({ { +1.0f, -1.0f }, {1.0f, 1.0f, 1.0f, 1.0f} ,{+1.0f, +1.0f} });

	D3D11_BUFFER_DESC bufDesc;

	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));

	bufDesc.ByteWidth = sizeof(UIVertex) * render_data_.vertex_list.size();
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourse;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &render_data_.vertex_list.at(0);
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDevice()->CreateBuffer(&bufDesc, &subResourse, render_data_.vertex_buffer.GetAddressOf());

	// 인덱스 작성

	render_data_.index_list.push_back(0);
	render_data_.index_list.push_back(1);
	render_data_.index_list.push_back(2);
	render_data_.index_list.push_back(2);
	render_data_.index_list.push_back(1);
	render_data_.index_list.push_back(3);

	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));

	bufDesc.ByteWidth = sizeof(DWORD) * render_data_.index_list.size();
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &render_data_.index_list.at(0);
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDevice()->CreateBuffer(&bufDesc, &subResourse, render_data_.index_buffer.GetAddressOf());
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

void RenderTarget::SetClearColor(XMFLOAT4 color)
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
	if (FAILED(hr = DX11APP->GetDevice()->CreateRenderTargetView(render_target_view_texture_.Get(), NULL, render_target_view_.GetAddressOf())))
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
	if (FAILED(hr = DX11APP->GetDevice()->CreateDepthStencilView(depth_stencil_view_texture_.Get(), &dsv_desc, depth_stencil_view_.GetAddressOf())))
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
	DX11APP->GetDeviceContext()->OMSetRenderTargets(1, render_target_view_.GetAddressOf(), depth_stencil_view_.Get());
	DX11APP->GetDeviceContext()->ClearRenderTargetView(render_target_view_.Get(), (FLOAT*)&clear_color_);
	DX11APP->GetDeviceContext()->ClearDepthStencilView(depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DX11APP->GetDeviceContext()->RSSetViewports(1, &view_port_);

	return false;
}

void RenderTarget::RenderingRT()
{
	DX11APP->SetBackBufferRTV();

	// Set Topology
	DX11APP->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Vertex Buffer
	UINT stride = sizeof(UIVertex);
	UINT offset = 0;
	DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, render_data_.vertex_buffer.GetAddressOf(), &stride, &offset);

	// Set Index Buffer
	DX11APP->GetDeviceContext()->IASetIndexBuffer(render_data_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);

	// Get Shader From ResourceMgr
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(render_data_.vs_id);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(render_data_.ps_id);

	if (vs == nullptr || ps == nullptr)
		return;

	// Set InputLayout
	DX11APP->GetDeviceContext()->IASetInputLayout(vs->InputLayout());

	// Set VS
	DX11APP->GetDeviceContext()->VSSetShader(vs->Get(), 0, 0);

	// Set PS
	DX11APP->GetDeviceContext()->PSSetShader(ps->Get(), 0, 0);

	// Reset GS
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);

	// Set Texture
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, render_target_view_srv_.GetAddressOf());

	// Draw Indexed
	DX11APP->GetDeviceContext()->DrawIndexed(render_data_.index_list.size(), 0, 0);
}

void RenderTarget::Resize(float width, float height)
{
	// Release Original RenderTarget
	render_target_view_.ReleaseAndGetAddressOf();
	depth_stencil_view_.ReleaseAndGetAddressOf();
	render_target_view_texture_.ReleaseAndGetAddressOf();
	depth_stencil_view_texture_.ReleaseAndGetAddressOf();
	render_target_view_srv_.ReleaseAndGetAddressOf();
	depth_stencil_view_srv_.ReleaseAndGetAddressOf();

	HRESULT hr;

	width_ = width;
	height_ = height;

	rect_.SetRectByCenter({ ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y / 2.0f }, ENGINE->GetWindowSize().x, ENGINE->GetWindowSize().y);

	SetViewPort();

	if (FAILED(hr = CreateRenderTargetView()))
		return;

	if (FAILED(hr = CreateDepthStencilView()))
		return;
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

bool RenderTargetMgr::DeletingRT(std::string rtname)
{
	auto iter = resdic_render_target_.find(rtname);
	if (iter != resdic_render_target_.end())
	{
		resdic_render_target_.erase("rtname");
		return true;
	}
	else
	{
		return false;
	}
}

void RenderTargetMgr::ResettingRT()
{
}

