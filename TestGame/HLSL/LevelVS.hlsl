#include "VertexCommon.hlsli"

struct VS_IN
{
    float3 p : F3_POSITION;
    float3 n : F3_NORMAL;
    float4 c : F4_COLOR;
    float2 t : F2_TEXTURE;
    float2 layer_texel : F2_TEXEL;
};

struct VS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL;
    float4 c : COLOR;
    float2 t : TEXTURE0;
    float2 layer_texel : F2_TEXEL;
    
    float lod : COLOR1;
};

VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 vLocal = float4(input.p, 1.0f);
	float4 vWorld = mul(vLocal, IdentityMatrix());
    float4 view_proj = mul(vWorld, ViewProjection());

    output.p = view_proj;
	output.n = input.n;
	output.t = input.t;

	return output;
}