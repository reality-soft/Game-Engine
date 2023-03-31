#include "include/VertexCommon.hlsli"

cbuffer CbShadowMatrix
{
    matrix shadow_matrix;
};

struct VS_IN
{
    float3 p : F3_POSITION;
    float3 n : F3_NORMAL;
    float2 t : F2_TEXTURE;
};

struct VS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD0;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    float4 local = float4(input.p, 1.0f);
    float4 world = mul(local, IdentityMatrix());
    float4 project = mul(world, shadow_matrix);

    output.p = project;
    output.n = input.n;
    output.t = input.t;

    return output;
}