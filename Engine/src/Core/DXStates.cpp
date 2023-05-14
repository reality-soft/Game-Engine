#include "stdafx.h"
#include "DXStates.h"
#include "DX11App.h"

using namespace reality;

ID3D11BlendState* reality::DXStates::bs_default()
{
    static ComPtr<ID3D11BlendState>        blend_state;

    if (blend_state.Get() == nullptr)
    {
        D3D11_BLEND_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.AlphaToCoverageEnable = false;
        bd.IndependentBlendEnable = false;

        bd.RenderTarget[0].BlendEnable = TRUE;

        // 알파 블랜딩 공식
        // Src : 이제 뿌려질 도형
        // Dest : 이미 뿌려진 Buffer
        // finalColor = SrcColor * SrcAlpha + DestColor * (1.0f-SrcAlpha)

        // RGB 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

        // Alpha 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

        // 마지막으로 계산된 알파값으로 어떤 성분을 뿌릴지 결정
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = DX11APP->GetDevice()->CreateBlendState(&bd, blend_state.GetAddressOf());

        if (blend_state.Get() == nullptr)
            assert(nullptr);

        return blend_state.Get();
    }

    else
        return blend_state.Get();
}

ID3D11BlendState* reality::DXStates::bs_alpha_blending()
{
    static ComPtr<ID3D11BlendState>        blend_state;

    if (blend_state.Get() == nullptr)
    {
        D3D11_BLEND_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.AlphaToCoverageEnable = false;
        bd.IndependentBlendEnable = false;

        bd.RenderTarget[0].BlendEnable = TRUE;

        // 알파 블랜딩 공식
        // Src : 이제 뿌려질 도형
        // Dest : 이미 뿌려진 Buffer
        // finalColor = SrcColor * SrcAlpha + DestColor * (1.0f-SrcAlpha)

        // RGB 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

        // Alpha 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

        // 마지막으로 계산된 알파값으로 어떤 성분을 뿌릴지 결정
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = DX11APP->GetDevice()->CreateBlendState(&bd, blend_state.GetAddressOf());

        if (blend_state.Get() == nullptr)
            assert(nullptr);

        return blend_state.Get();
    }

    else
        return blend_state.Get();
}


ID3D11BlendState* reality::DXStates::bs_dual_source_blend()
{
    static ComPtr<ID3D11BlendState>		blend_state;

    if (blend_state.Get() == nullptr)
    {
        D3D11_BLEND_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.AlphaToCoverageEnable = false;
        bd.IndependentBlendEnable = TRUE;

        bd.RenderTarget[0].BlendEnable = TRUE;

        // 알파 블랜딩 공식
        // Src : 이제 뿌려질 도형
        // Dest : 이미 뿌려진 Buffer
        // finalColor = SrcColor * SrcAlpha + DestColor * (1.0f-SrcAlpha)

        // RGB 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_SRC1_COLOR;
        // Alpha 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

        // 마지막으로 계산된 알파값으로 어떤 성분을 뿌릴지 결정
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = DX11APP->GetDevice()->CreateBlendState(&bd, blend_state.GetAddressOf());

        if (blend_state.Get() == nullptr)
            assert(nullptr);

        return blend_state.Get();
    }

    else
        return blend_state.Get();
}

ID3D11BlendState* reality::DXStates::bs_blend_higher_rgb()
{
    static ComPtr<ID3D11BlendState>		blend_state;

    if (blend_state.Get() == nullptr)
    {
        D3D11_BLEND_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.AlphaToCoverageEnable = false;
        bd.IndependentBlendEnable = false;


        // 알파 블랜딩 공식
        // Src : 이제 뿌려질 도형
        // Dest : 이미 뿌려진 Buffer
        // finalColor = SrcColor * SrcAlpha + DestColor * (1.0f-SrcAlpha)

        // RGB 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendEnable = TRUE;
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        // Alpha 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

        // 마지막으로 계산된 알파값으로 어떤 성분을 뿌릴지 결정
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = DX11APP->GetDevice()->CreateBlendState(&bd, blend_state.GetAddressOf());

        if (blend_state.Get() == nullptr)
            assert(nullptr);

        return blend_state.Get();
    }

    else
        return blend_state.Get();
}

ID3D11BlendState* reality::DXStates::bs_alpha_to_coverage_enable()
{
    static ComPtr<ID3D11BlendState>		blend_state;

    if (blend_state.Get() == nullptr)
    {
        D3D11_BLEND_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.AlphaToCoverageEnable = true;
        bd.IndependentBlendEnable = false;

        bd.RenderTarget[0].BlendEnable = TRUE;

        // 알파 블랜딩 공식
        // Src : 이제 뿌려질 도형
        // Dest : 이미 뿌려진 Buffer
        // finalColor = SrcColor * SrcAlpha + DestColor * (1.0f-SrcAlpha)

        // RGB 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        // Alpha 성분을 혼합하는 명령
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

        // 마지막으로 계산된 알파값으로 어떤 성분을 뿌릴지 결정
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = DX11APP->GetDevice()->CreateBlendState(&bd, blend_state.GetAddressOf());

        if (blend_state.Get() == nullptr)
            assert(nullptr);

        return blend_state.Get();
    }

    else
        return blend_state.Get();
}

ID3D11DepthStencilState* reality::DXStates::ds_defalut()
{
    static ComPtr<ID3D11DepthStencilState> depth_stencil_state;

    if (depth_stencil_state.Get() == nullptr)
    {
        // 깊이버퍼 상태값 세팅
        D3D11_DEPTH_STENCIL_DESC dsDescDepth;
        ZeroMemory(&dsDescDepth, sizeof(D3D11_DEPTH_STENCIL_DESC));
        dsDescDepth.DepthEnable = TRUE;
        dsDescDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDescDepth.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        // 스텐실 상태값 세팅
        dsDescDepth.StencilEnable = FALSE;
        dsDescDepth.StencilReadMask = 1;
        dsDescDepth.StencilWriteMask = 1;

        // 디폴트 값
        dsDescDepth.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDescDepth.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDescDepth.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDescDepth.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

        HRESULT hr = DX11APP->GetDevice()->CreateDepthStencilState(&dsDescDepth, depth_stencil_state.GetAddressOf());

        if (depth_stencil_state.Get() == nullptr)
            assert(nullptr);

        return depth_stencil_state.Get();
    }
    else
	    return depth_stencil_state.Get();
}

ID3D11DepthStencilState* reality::DXStates::ds_depth_disable()
{
    static ComPtr<ID3D11DepthStencilState> depth_stencil_state;

    if (depth_stencil_state.Get() == nullptr)
    {
        // 깊이버퍼 상태값 세팅
        D3D11_DEPTH_STENCIL_DESC dsDescDepth;
        ZeroMemory(&dsDescDepth, sizeof(D3D11_DEPTH_STENCIL_DESC));
        dsDescDepth.DepthEnable = FALSE;
        dsDescDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDescDepth.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        // 스텐실 상태값 세팅
        dsDescDepth.StencilEnable = FALSE;
        dsDescDepth.StencilReadMask = 1;
        dsDescDepth.StencilWriteMask = 1;

        // 디폴트 값
        dsDescDepth.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDescDepth.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDescDepth.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDescDepth.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

        HRESULT hr = DX11APP->GetDevice()->CreateDepthStencilState(&dsDescDepth, depth_stencil_state.GetAddressOf());

        if (depth_stencil_state.Get() == nullptr)
            assert(nullptr);

        return depth_stencil_state.Get();
    }
    else
        return depth_stencil_state.Get();
}

ID3D11DepthStencilState* reality::DXStates::ds_depth_enable_no_write()
{
    static ComPtr<ID3D11DepthStencilState> depth_stencil_state;

    if (depth_stencil_state.Get() == nullptr)
    {
        // 깊이버퍼 상태값 세팅
        D3D11_DEPTH_STENCIL_DESC dsDescDepth;
        ZeroMemory(&dsDescDepth, sizeof(D3D11_DEPTH_STENCIL_DESC));
        dsDescDepth.DepthEnable = TRUE;
        dsDescDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDescDepth.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        // 스텐실 상태값 세팅
        dsDescDepth.StencilEnable = FALSE;
        dsDescDepth.StencilReadMask = 1;
        dsDescDepth.StencilWriteMask = 1;

        // 디폴트 값
        dsDescDepth.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDescDepth.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDescDepth.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDescDepth.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

        HRESULT hr = DX11APP->GetDevice()->CreateDepthStencilState(&dsDescDepth, depth_stencil_state.GetAddressOf());

        if (depth_stencil_state.Get() == nullptr)
            assert(nullptr);

        return depth_stencil_state.Get();
    }
    else
        return depth_stencil_state.Get();
}


ID3D11RasterizerState* reality::DXStates::rs_solid_cull_none()
{
    static ComPtr<ID3D11RasterizerState>		rasterizer_state;

    if (rasterizer_state.Get() == nullptr)
    {
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));

        rd.DepthClipEnable = true;
        rd.CullMode = D3D11_CULL_NONE;
        rd.FillMode = D3D11_FILL_SOLID;

        HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rd, rasterizer_state.GetAddressOf());

        if (rasterizer_state.Get() == nullptr)
            assert(nullptr);

        return rasterizer_state.Get();
    }
    else
        return rasterizer_state.Get();
}

ID3D11RasterizerState* reality::DXStates::rs_solid_cull_back()
{
    static ComPtr<ID3D11RasterizerState>		rasterizer_state;

    if (rasterizer_state.Get() == nullptr)
    {
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));

        rd.DepthClipEnable = true;
        rd.CullMode = D3D11_CULL_BACK;
        rd.FillMode = D3D11_FILL_SOLID;

        HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rd, rasterizer_state.GetAddressOf());

        if (rasterizer_state.Get() == nullptr)
            assert(nullptr);

        return rasterizer_state.Get();
    }
    else
        return rasterizer_state.Get();
}

ID3D11RasterizerState* reality::DXStates::rs_solid_cull_front()
{
    static ComPtr<ID3D11RasterizerState>		rasterizer_state;

    if (rasterizer_state.Get() == nullptr)
    {
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));

        rd.DepthClipEnable = true;
        rd.CullMode = D3D11_CULL_FRONT;
        rd.FillMode = D3D11_FILL_SOLID;

        HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rd, rasterizer_state.GetAddressOf());

        if (rasterizer_state.Get() == nullptr)
            assert(nullptr);

        return rasterizer_state.Get();
    }
    else
        return rasterizer_state.Get();
}

ID3D11RasterizerState* reality::DXStates::rs_wireframe_cull_none()
{
    static ComPtr<ID3D11RasterizerState>		rasterizer_state;

    if (rasterizer_state.Get() == nullptr)
    {
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));

        rd.DepthClipEnable = true;
        rd.CullMode = D3D11_CULL_NONE;
        rd.FillMode = D3D11_FILL_WIREFRAME;

        HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rd, rasterizer_state.GetAddressOf());

        if (rasterizer_state.Get() == nullptr)
            assert(nullptr);

        return rasterizer_state.Get();
    }
    else
        return rasterizer_state.Get();
}

ID3D11RasterizerState* reality::DXStates::rs_wireframe_cull_back()
{
    static ComPtr<ID3D11RasterizerState>		rasterizer_state;

    if (rasterizer_state.Get() == nullptr)
    {
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));

        rd.DepthClipEnable = true;
        rd.CullMode = D3D11_CULL_BACK;
        rd.FillMode = D3D11_FILL_WIREFRAME;

        HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rd, rasterizer_state.GetAddressOf());

        if (rasterizer_state.Get() == nullptr)
            assert(nullptr);

        return rasterizer_state.Get();
    }
    else
        return rasterizer_state.Get();
}

ID3D11RasterizerState* reality::DXStates::rs_wireframe_cull_front()
{
    static ComPtr<ID3D11RasterizerState>		rasterizer_state;

    if (rasterizer_state.Get() == nullptr)
    {
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));

        rd.DepthClipEnable = true;
        rd.CullMode = D3D11_CULL_FRONT;
        rd.FillMode = D3D11_FILL_WIREFRAME;

        HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rd, rasterizer_state.GetAddressOf());

        if (rasterizer_state.Get() == nullptr)
            assert(nullptr);

        return rasterizer_state.Get();
    }
    else
        return rasterizer_state.Get();
}

ID3D11SamplerState* reality::DXStates::ss_defalut()
{

    static ComPtr<ID3D11SamplerState>		sampler_state;
    if (sampler_state.Get() == nullptr)
    {
        D3D11_SAMPLER_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        //sd.MipLODBias = 0;
        //sd.MaxAnisotropy = 0;
        //sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS; // 필터에 따라 필수
        //sd.BorderColor[0] = 0;
        //sd.BorderColor[1] = 0;
        //sd.BorderColor[2] = 0;
        //sd.BorderColor[3] = 0;
        //sd.MinLOD = -FLT_MAX;   // 밉맵의 범위 하한치 0이 최대이며 가장 세부적인 밉맵 레벨을 나타낸다.
        //sd.MaxLOD = FLT_MAX;    // 밉맵의 범위 상한치

        HRESULT hr = DX11APP->GetDevice()->CreateSamplerState(&sd, sampler_state.GetAddressOf());
        return sampler_state.Get();
    }
    else
        return sampler_state.Get();
}
