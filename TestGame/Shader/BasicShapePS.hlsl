struct PS_IN
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
	float4 i : INDEX;
	float4 w : WEIGHT;
};
struct PS_OUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float4 vWorld : TEXCOORD1;
	float3 vLight : TEXCOORD2;
};

Texture2D    g_txTex			: register(t0);
SamplerState g_SampleWrap		: register(s0);

float4 PS(PS_OUT input) : SV_Target
{
	return input.c;
}