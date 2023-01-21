#include "common.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dinput.h>

#include "CommonStates.h"

class DX11App
{
	SINGLETON(DX11App);

private:
    //=======================================================디바이스
    ComPtr<IDXGIFactory>           dxgi_factory;

    ComPtr<ID3D11Device>           dx11_device;
    ComPtr<ID3D11DeviceContext>    dx11_context;
    ComPtr<IDXGISwapChain>         dx11_swap_chain;

    ComPtr<ID3D11RenderTargetView> dx11_rtview;
    ComPtr<ID3D11DepthStencilView> dx11_dsview;

    D3D11_VIEWPORT                 view_port;
    DirectX::CommonStates dx_states = DirectX::CommonStates(dx11_device.Get());

public:
    float clear_color[4] = { 0.03, 0.03, 0.03, 1 };

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

public:
    bool OnInit(POINT buffer_size, HWND hwnd);

    void PreRender(bool solid_on, bool alpha_on, bool depth_on);
    void PostRender(bool vsync_on);

    bool OnResize(HWND hwnd, UINT new_x, UINT new_y);
};