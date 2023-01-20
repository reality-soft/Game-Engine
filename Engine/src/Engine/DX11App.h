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

    DirectX::CommonStates DXStates;

public:
    D3D11_VIEWPORT                 viewPort;
    float clearcolor[4] = { 0.03, 0.03, 0.03, 1 };

};