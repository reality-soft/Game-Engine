#include "include/VertexCommon.hlsli"

struct VS_IN
{
	float3 p : F3_POSITION;
	float3 n : F3_NORMAL;
	float4 c : F4_COLOR;
	float2 t : F2_TEXTURE;
	float4 i : F4_INDEX;
	float4 w : F4_WEIGHT;
};
struct VS_OUT
{
	float4 p : SV_POSITION;
	float4 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float lod : TEXCOORD1;
};

cbuffer cb_transform_data : register(b1)
{
	matrix g_matWorld;
};

cbuffer cb_bone_data : register(b2)
{
	matrix g_matBone[255];
}

VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	float4 vLocal = float4(input.p, 1.0f);

	float4 animation = 0;
	float4 animNormal = 0;

	for (int s = 0; s < 4; s++)
	{
		uint skeletonIndex = input.i[s];
		float weight = input.w[s];

		animation += mul(vLocal, g_matBone[skeletonIndex]) * weight;
		animNormal += mul(input.n, g_matBone[skeletonIndex]) * weight;
	}

	float4 vWorld = mul(animation, g_matWorld);
	float4 vView = mul(vWorld, view_matrix);
	float4 vProj = mul(vView, projection_matrix);

	output.lod = 0;
	output.p = vProj;
	output.n = animNormal;
	output.c = input.c;
	output.t = input.t;

	//output.vLight = float3(0, 0, -1);

	return output;
}