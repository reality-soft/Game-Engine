#include "common.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include "CommonStates.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class DX11App
{
	SINGLETON(DX11App);

private:
    //=======================================================디바이스
    ComPtr<IDXGIFactory>           dxgiFactory = nullptr;

    ComPtr<ID3D11Device>           dx11Device = nullptr;
    ComPtr<ID3D11DeviceContext>    dx11Context = nullptr;
    ComPtr<IDXGISwapChain>         dx11SwapChain = nullptr;

    ComPtr<ID3D11RenderTargetView> dx11RTView = nullptr;
    ComPtr<ID3D11DepthStencilView> dx11DSView = nullptr;

    D3D11_VIEWPORT                 viewPort;
    DirectX::CommonStates DXStates;

public:
    float clearcolor[4] = { 0.03, 0.03, 0.03, 1 };

    //=======================================================값반환 함수
    ID3D11Device* GetDevice() { return dx11Device.Get(); }
    ID3D11DeviceContext* GetDeviceContext() { return dx11Context.Get(); }
    IDXGIFactory* GetFactory() { return dxgiFactory.Get(); }
    IDXGISwapChain* GetSwapChain() { return dx11SwapChain.Get(); }

    ID3D11RenderTargetView* GetRenderTargetView() { return dx11RTView.Get(); }
    ID3D11RenderTargetView** GetRenderTargetViewAddress() { return dx11RTView.GetAddressOf(); }

    ID3D11DepthStencilView* GetDepthStencilView() { return dx11DSView.Get(); }
    ID3D11DepthStencilView** GetDepthStencilViewAddress() { return dx11DSView.GetAddressOf(); }

    D3D11_VIEWPORT          GetViewPort() { return viewPort; }
    D3D11_VIEWPORT* GetViewPortAddress() { return &viewPort; }
};