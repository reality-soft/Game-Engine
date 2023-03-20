#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float4 n : NORMAL;
    float4 c : COLOR;
    float2 t : TEXCOORD;
    float lod : TEXCOORD1;
};

Texture2D textures[7] : register(t0);
SamplerState sample : register(s0);

float4 PS(PS_OUT output) : SV_Target
{
    // VertexColor
    float4 base_color = CreateColor(textures[0], sample, output.t);

    return base_color;
}