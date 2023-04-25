#pragma once
#include "CommonStates.h"

class DLL_API DX11App
{
	SINGLETON(DX11App)
#define DX11APP DX11App::GetInst()

public:
    float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.f };
    //=======================================================값반환 함수
    ID3D11Device* GetDevice() { return dx11_device.Get(); }
    ID3D11DeviceContext* GetDeviceContext() { return dx11_context.Get(); }
    IDXGIFactory* GetFactory() { return dxgi_factory.Get(); }
    IDXGISwapChain* GetSwapChain() { return dx11_swap_chain.Get(); }

    ID3D11RenderTargetView* GetRenderTargetView() { return dx11_rtview.Get(); }
    ID3D11RenderTargetView** GetRenderTargetViewAddress() { return dx11_rtview.GetAddressOf(); }

    ID3D11DepthStencilView* GetDepthStencilView() { return dx11_dsview.Get(); }
    ID3D11DepthStencilView** GetDepthStencilViewAddress() { return dx11_dsview.GetAddressOf(); }

    D3D11_VIEWPORT          GetViewPort() { return view_port; }
    D3D11_VIEWPORT* GetViewPortAddress() { return &view_port; }
    DirectX::CommonStates* GetCommonStates() { return dx_states; }

    void SetWireframes();
    void SetSolid();

public:
    bool OnInit(POINT buffer_size, HWND hwnd);

    void PreRender(bool solid_on, bool alpha_on, bool depth_on);
    void PostRender(bool vsync_on);

    bool Resize(UINT new_x, UINT new_y);

    void OnRelease();

    void CreatePreProcess();

    void SetBackBufferRTV();

private:
    //=======================================================디바이스
    ComPtr<IDXGIFactory>           dxgi_factory;

    ComPtr<ID3D11Device>           dx11_device;
    ComPtr<ID3D11DeviceContext>    dx11_context;
    ComPtr<IDXGISwapChain>         dx11_swap_chain;

    ComPtr<ID3D11RenderTargetView> dx11_rtview;
    ComPtr<ID3D11DepthStencilView> dx11_dsview;
    DirectX::CommonStates*         dx_states;

    D3D11_VIEWPORT                 view_port;
};


static XMMATRIX InterpolByTime(XMMATRIX& a, XMMATRIX& b, float t)
{
    XMMATRIX result;

    for (int i = 0; i < 4; i++)
    {
        XMVECTOR row_a = XMLoadFloat4((XMFLOAT4*)&a.r[i]);
        XMVECTOR row_b = XMLoadFloat4((XMFLOAT4*)&b.r[i]);
        XMVECTOR row_result = XMVectorLerp(row_a, row_b, t);
        XMStoreFloat4((XMFLOAT4*)&result.r[i], row_result);
    }

    return result;
}

