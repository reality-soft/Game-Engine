#include "include/EffectCommonHeaderPS.hlsli"

float4 PS(PS_IN input) : SV_Target
{
	// Tex
	float4 tex_color = g_txOpacity.Sample(g_SampleWrap, input.uv);

	float alpha = max(max(tex_color.r, tex_color.g), tex_color.b);

	if (alpha < 0.1f)
	{
		discard;
	}

	if (tex_color.r == 1.0f && tex_color.g == 1.0f && tex_color.b == 1.0f)
		return float4(1.0f, 1.0f, 1.0f, alpha * input.c.a);

	float4 final_color = float4(tex_color.rgb * input.c.rgb, alpha * input.c.a);
	return float4(final_color.rgb * 2.0f, final_color.a);
}