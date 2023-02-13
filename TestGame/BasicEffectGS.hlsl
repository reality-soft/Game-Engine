cbuffer cb_data : register(b0)
{
	matrix g_matWorld : packoffset(c0);
	matrix g_matView : packoffset(c4);
	matrix g_matProj : packoffset(c8);
};

struct GS_IN
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float4 vWorld : TEXCOORD1;
	float3 vLight : TEXCOORD2;
};

struct GS_OUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float4 vWorld : TEXCOORD1;
	float3 vLight : TEXCOORD2;
};

[maxvertexcount(4)]
void GS(
	in point GS_IN input[1] : SV_POSITION,
	inout TriangleStream< GS_OUT > output)
{
	// TriangleStrip으로 구성한다
	// p 설정
	const float3 g_positions[4] =
	{
		float3(-1.0, +1.0, 0.0),
		float3(+1.0, +1.0, 0.0),
		float3(-1.0, -1.0, 0.0),
		float3(+1.0, -1.0, 0.0),
	};

	// t 설정
	const float2 g_texcoords[4] =
	{
		float2(0.0, 0.0),
		float2(1.0, 0.0),
		float2(0.0, 1.0),
		float2(1.0, 1.0),
	};

	GS_OUT vertex;
	for (int i = 0; i < 4; i++)
	{
		vertex.p = float4(input[0].p.xyz + g_positions[i].xyz, 1.0f);
		vertex.c = input[0].c;
		vertex.t = g_texcoords[i];
		vertex.n = input[0].n;
		vertex.vWorld = input[0].vWorld;
		vertex.vLight = input[0].vLight;

		float4 World = mul(vertex.p, g_matWorld);
		float4 View = mul(World, g_matView);
		vertex.p = mul(View, g_matProj);

		output.Append(vertex);
	}
	output.RestartStrip();

}