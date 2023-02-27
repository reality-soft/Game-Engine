// Global Directional Lighting
cbuffer cb_light : register(b0)
{
    float4 direction;
    float4 color;
}

// White Basic color
float4 WhiteColor()
{
    return float4(1, 1, 1, 1);
}

// Returns Color with Texture, returns white color if no texture
float4 CreateColor(Texture2D tex, SamplerState sample, float2 uv)
{
    float4 color = tex.Sample(sample, uv);
    if (length(color) == 0)
        return WhiteColor();
    
    return color;
}
