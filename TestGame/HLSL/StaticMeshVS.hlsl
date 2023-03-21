#include "include/VertexCommon.hlsli"

cbuffer cb_data : register(b1)
{
    matrix world_matrix;
};

struct VS_IN
{
    float3 p : F3_POSITION;
    float3 n : F3_NORMAL;
    float4 c : F4_COLOR;
    float2 t : F2_TEXTURE;
};
struct VS_OUT
{
    float4 p : SV_POSITION;
    float4 n : NORMAL;
    float4 c : COLOR;
    float2 t : TEXCOORD;
    float lod : TEXCOORD1;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    float4 local = float4(input.p, 1.0f);
    float4 normal = float4(input.n, 1.0f);

    float4 world = mul(local, world_matrix);
    float4 projection = mul(world, ViewProjection());

    output.lod = GetLod(input.p);

    output.p = projection;
    output.n = normal;
    output.c = input.c;
    output.t = input.t;

    return output;
}