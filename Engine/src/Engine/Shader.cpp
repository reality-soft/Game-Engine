#include "stdafx.h"
#include "Shader.h"

using namespace KGCA41B;

bool VertexShader::LoadCompiled(wstring cso_file)
{
    HRESULT hr;
    ID3DBlob* blob = nullptr;

    hr = D3DReadFileToBlob(cso_file.c_str(), &blob);

    hr = DX11APP->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, vs.GetAddressOf());


    ID3D11ShaderReflection* reflection = nullptr;
    hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);

    D3D11_SHADER_DESC shader_desc;
    hr = reflection->GetDesc(&shader_desc);

    vector<D3D11_INPUT_ELEMENT_DESC> ied;
    UINT byte_offset = 0;

    for (UINT i = 0; i < shader_desc.InputParameters; i++)
    {
        D3D11_INPUT_ELEMENT_DESC input_desc;
        ZeroMemory(&input_desc, sizeof(input_desc));

        D3D11_SIGNATURE_PARAMETER_DESC param_desc;
        hr = reflection->GetInputParameterDesc(i, &param_desc);
        if (SUCCEEDED(hr))
        {
            input_desc.SemanticName = param_desc.SemanticName;
            input_desc.SemanticIndex = param_desc.SemanticIndex;
            input_desc.AlignedByteOffset = byte_offset;

            if (string(param_desc.SemanticName).find("F2") != string::npos)
            {
                input_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
                byte_offset += 8;
            }

            if (string(param_desc.SemanticName).find("F3") != string::npos)
            {
                input_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                byte_offset += 12;
            }

            if (string(param_desc.SemanticName).find("F4") != string::npos)
            {
                input_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                byte_offset += 16;
            }

            ied.push_back(input_desc);
        }
    }

    hr = DX11APP->GetDevice()->CreateInputLayout(ied.data(), ied.size(), blob->GetBufferPointer(), blob->GetBufferSize(), input_layout.GetAddressOf());

    blob->Release();
    blob = nullptr;

    return true;
}

ID3D11VertexShader* VertexShader::Get()
{
	return vs.Get();
}

ID3D11InputLayout* VertexShader::InputLayoyt()
{
    return input_layout.Get();
}

bool PixelShader::LoadCompiled(wstring cso_file)
{
    HRESULT hr = S_OK;

    ID3DBlob* blob = nullptr;
    hr = D3DReadFileToBlob(cso_file.c_str(), &blob);

    hr = DX11APP->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ps.GetAddressOf());

    blob->Release();
    blob = nullptr;

    return true;
}

ID3D11PixelShader* PixelShader::Get()
{
	return ps.Get();
}

bool KGCA41B::GeometryShader::LoadCompiled(wstring _csoFIle)
{
    HRESULT hr = S_OK;

    ID3DBlob* blob = nullptr;
    hr = D3DReadFileToBlob(_csoFIle.c_str(), &blob);

    hr = DX11APP->GetDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, gs.GetAddressOf());

    blob->Release();
    blob = nullptr;

    return true;
}

ID3D11GeometryShader* GeometryShader::Get()
{
    return gs.Get();
}
