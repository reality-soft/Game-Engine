struct VS_IN
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
	float4 i : INDEX;
	float4 w : WEIGHT;
};
struct VS_OUT
{
	float4 p : SV_POSITION;
	float4 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float3 vLight : TEXCOORD1;
};

cbuffer cb_data : register(b0)
{
	matrix g_matWorld;
	matrix g_matView;
	matrix g_matProj;
};

cbuffer cb_bone_data : register(b1)
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
	float4 vView = mul(vWorld, g_matView);
	float4 vProj = mul(vView, g_matProj);

	output.p = vProj;
	output.n = animNormal;
	output.c = input.c;
	output.t = input.t;

	output.vLight = float3(0, 0, -1);

	return output;
}