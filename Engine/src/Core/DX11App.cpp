#include "stdafx.h"
#include "DX11App.h"
#include "RenderTargetMgr.h"

void DX11App::SetWireframes()
{
    dx11_context->RSSetState(dx_states->Wireframe());
}

void DX11App::SetSolid()
{
    dx11_context->RSSetState(dx_states->CullNone());
}

bool DX11App::OnInit(POINT buffer_size, HWND hwnd)
{
    HRESULT hr;

    // 팩토리 생성

    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)dxgi_factory.GetAddressOf());
    if (FAILED(hr))
        return false;

    // 스왑체인 및 디바이스 생성

    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = hwnd;
    swap_chain_desc.Windowed = true;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    swap_chain_desc.BufferDesc.Width = buffer_size.x;
    swap_chain_desc.BufferDesc.Height = buffer_size.y;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_desc.SampleDesc.Count = 4;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_FEATURE_LEVEL feature_levels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        feature_levels,
        1,
        D3D11_SDK_VERSION,
        &swap_chain_desc,
        dx11_swap_chain.GetAddressOf(),
        dx11_device.GetAddressOf(),
        NULL,
        dx11_context.GetAddressOf()
    );
    if (FAILED(hr))
        return false;


    // 스왑체인으로부터 얻어온 텍스처로 렌더타깃 뷰 생성

    ID3D11Texture2D* rt_texture;
    hr = dx11_swap_chain.Get()->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (void**)(&rt_texture));
    if (FAILED(hr))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC rt_desc;
    ZeroMemory(&rt_desc, sizeof(rt_desc));
    rt_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rt_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

    hr = dx11_device.Get()->CreateRenderTargetView(rt_texture, &rt_desc, dx11_rtview.GetAddressOf());
    if (FAILED(hr))
        return false;

    rt_texture->Release();

    // 깊이_스텐실 버퍼 생성

    ID3D11Texture2D* ds_texture;
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = buffer_size.x;
    texture_desc.Height = buffer_size.y;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texture_desc.SampleDesc.Count = 4;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = dx11_device->CreateTexture2D(&texture_desc, 0, &ds_texture);
    if (FAILED(hr))
        return false;

    // 깊이_스텐실 뷰 생성

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    hr = dx11_device->CreateDepthStencilView(ds_texture, &dsv_desc, dx11_dsview.GetAddressOf());
    if (FAILED(hr))
        return false;

    ds_texture->Release();

    dx_states = new DirectX::CommonStates(dx11_device.Get());

    view_port.Width = (float)buffer_size.x;
    view_port.Height = (float)buffer_size.y;
    view_port.MinDepth = 0.0f;
    view_port.MaxDepth = 1.0f;
    view_port.TopLeftX = 0.0f;
    view_port.TopLeftY = 0.0f;
    dx11_context.Get()->RSSetViewports(1, &view_port);

	return true;
}

void DX11App::PreRender(bool solid_on, bool alpha_on, bool depth_on)
{
    if (alpha_on)
        dx11_context->OMSetBlendState(dx_states->AlphaBlend(), 0, -1);

    if (depth_on)
        dx11_context->OMSetDepthStencilState(dx_states->DepthDefault(), 0xff);

    if (solid_on)
        dx11_context->RSSetState(dx_states->CullNone());
    else
        dx11_context->RSSetState(dx_states->Wireframe());

    dx11_context->OMSetRenderTargets(1, dx11_rtview.GetAddressOf(), dx11_dsview.Get());
    dx11_context->ClearRenderTargetView(GetRenderTargetView(), clear_color);
    dx11_context->ClearDepthStencilView(GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1, 0);
    dx11_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DX11App::PostRender(bool _vsyncOn)
{
    if (_vsyncOn)
        dx11_swap_chain.Get()->Present(1, 0);
    else
        dx11_swap_chain.Get()->Present(0, 0);
}

bool DX11App::Resize(UINT new_x, UINT new_y)
{
    if (dx11_device == nullptr)
        return false;

    HRESULT hr;

    // 렌더타깃 및 연동된 버퍼 해제
    dx11_context.Get()->OMSetRenderTargets(0, nullptr, nullptr);
    dx11_rtview.ReleaseAndGetAddressOf();
    dx11_dsview.ReleaseAndGetAddressOf();

    //dx11_context.Get()->ClearState();
    //dx11_context.Get()->Flush();
    
    // 백버퍼 리사이즈
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    hr = dx11_swap_chain.Get()->GetDesc(&swap_chain_desc);
    hr = dx11_swap_chain.Get()->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

    // 백버퍼의 텍스처로 새로운 렌더타깃 뷰 생성
    D3D11_RENDER_TARGET_VIEW_DESC rt_desc;
    ZeroMemory(&rt_desc, sizeof(rt_desc));
    rt_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rt_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;


    ComPtr<ID3D11Texture2D> rt_texture;
    hr = dx11_swap_chain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)rt_texture.GetAddressOf());
    hr = dx11_device.Get()->CreateRenderTargetView(rt_texture.Get(), &rt_desc, dx11_rtview.GetAddressOf());


    // 깊이_스텐실 버퍼 생성
    ComPtr<ID3D11Texture2D> ds_texture;
    DXGI_SWAP_CHAIN_DESC scd;
    dx11_swap_chain.Get()->GetDesc(&scd);
    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = scd.BufferDesc.Width;
    td.Height = scd.BufferDesc.Height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.SampleDesc.Count = 4;
    td.SampleDesc.Quality = 0;
    td.Format = DXGI_FORMAT_R24G8_TYPELESS;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = dx11_device.Get()->CreateTexture2D(&td, NULL, ds_texture.GetAddressOf());

    // 깊이_스텐실 뷰 생성.
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    hr = dx11_device.Get()->CreateDepthStencilView(ds_texture.Get(), &dsvDesc, dx11_dsview.GetAddressOf());

    // 뷰포트 생성
    view_port.Width =  (float)new_x;
    view_port.Height = (float)new_y;
    view_port.MinDepth = 0.0f;
    view_port.MaxDepth = 1.0f;
    view_port.TopLeftX = 0.0f;
    view_port.TopLeftY = 0.0f;
    dx11_context.Get()->RSSetViewports(1, &view_port);

    // UI 랜더타겟 리사이즈
    reality::RENDER_TARGET->LoadRT("UI")->Resize((float)new_x, (float)new_y);

    return true;
}

void DX11App::OnRelease()
{
    if (dx_states)
    {
        delete dx_states;
        dx_states = nullptr;
    }
}

void DX11App::CreatePreProcess()
{


}

void DX11App::SetBackBufferRTV()
{
    UINT numViewPorts = 1;

    ID3D11RenderTargetView* pNullRTV = NULL;
    ID3D11DepthStencilView* pNullDSV = NULL;
    DX11APP->GetDeviceContext()->OMSetRenderTargets(1, &pNullRTV, pNullDSV);
    DX11APP->GetDeviceContext()->OMSetRenderTargets(1, dx11_rtview.GetAddressOf(), dx11_dsview.Get());
    DX11APP->GetDeviceContext()->RSSetViewports(1, GetViewPortAddress());
}
