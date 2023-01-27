#include "Shader.h"

using namespace KGCA41B;

bool VsDefault::LoadCompiled(wstring cso_file)
{
    HRESULT hr;
    ID3DBlob* blob = nullptr;

    hr = D3DReadFileToBlob(cso_file.c_str(), &blob);

    hr = DX11APP->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, vs.GetAddressOf());

    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0,12,D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,24,D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT,    0,40,D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    hr = DX11APP->GetDevice()->CreateInputLayout(ied, ARRAYSIZE(ied), blob->GetBufferPointer(), blob->GetBufferSize(), input_layout.GetAddressOf());

    blob->Release();
    blob = nullptr;

    return true;
}

ID3D11VertexShader* VsDefault::Get()
{
	return vs.Get();
}

bool VsSkinned::LoadCompiled(wstring cso_file)
{
    HRESULT hr;
    ID3DBlob* blob = nullptr;

    hr = D3DReadFileToBlob(cso_file.c_str(), &blob);

    hr = DX11APP->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, vs.GetAddressOf());

    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0,12,D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,24,D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT,    0,40,D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "INDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    hr = DX11APP->GetDevice()->CreateInputLayout(ied, ARRAYSIZE(ied), blob->GetBufferPointer(), blob->GetBufferSize(), input_layout.GetAddressOf());

    blob->Release();
    blob = nullptr;

    return true;
}

bool PsDefault::LoadCompiled(wstring cso_file)
{
    HRESULT hr = S_OK;

    ID3DBlob* blob = nullptr;
    hr = D3DReadFileToBlob(cso_file.c_str(), &blob);

    hr = DX11APP->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ps.GetAddressOf());

    blob->Release();
    blob = nullptr;

    return true;
}

ID3D11PixelShader* PsDefault::Get()
{
	return ps.Get();
}
