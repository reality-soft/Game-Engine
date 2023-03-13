struct PS_IN
{
    float4 p : SV_POSITION;
    float2 t : TEXCOORD0;
};

cbuffer cb_light : register(b0)
{
    float4 default_light;
    float light_bright;
}

Texture2D       g_tex : register(t0);
SamplerState    g_SampleWrap : register(s0);

float4 PS(PS_IN input) : SV_Target
{
    float4 tex_color = g_tex.Sample(g_SampleWrap, input.t);

    return tex_color;
}
