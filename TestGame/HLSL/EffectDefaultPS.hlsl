#include "include/EffectCommonHeaderPS.hlsli"


float4 PS(PS_IN input) : SV_Target
{
	float4 tex_color = g_txDiffuse.Sample(g_SampleWrap, input.t);

	return tex_color;
}