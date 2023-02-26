#include "LevelHeader.hlsli"

cbuffer cb_data : register(b0)
{
	matrix g_matWorld;
};

cbuffer cb_viewproj : register(b1)
{
	matrix g_matView;
	matrix g_matProj;
}

VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 vLocal = float4(input.p, 1.0f);
	float4 vWorld = mul(vLocal, IdentityMatrix());
	float4 vView = mul(vWorld, g_matView);
	float4 vProj = mul(vView, g_matProj);

	output.p = vProj;
	output.n = input.n;
	output.t = input.t;

	return output;
}