struct PS_OUT
{
	float4 p : SV_POSITION;
	float4 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD;
};

cbuffer cb_light : register(b0)
{
	float4 direction;
	float4 color;
}

Texture2D    diffuse			: register(t0);
Texture2D    normalmap			: register(t1);
Texture2D    metalic			: register(t2);
Texture2D    roughness			: register(t3);
Texture2D    ambient			: register(t4);

SamplerState samper_state		: register(s0);

float4 PS(PS_OUT output) : SV_Target
{
	// Tex
	float4 base_color = diffuse.Sample(samper_state, output.t);
	float4 normal_color = normalmap.Sample(samper_state, output.t);
	float4 metalic_color = metalic.Sample(samper_state, output.t);
	float4 roughness_color = roughness.Sample(samper_state, output.t);
	float4 ambient_color = ambient.Sample(samper_state, output.t);

	// Light
	float4 light = -direction * color;
	float bright = max(0.2f, dot(output.n, light));
	float4 normal = { bright , bright , bright , 1 };

	return output.c * base_color * normal;
}