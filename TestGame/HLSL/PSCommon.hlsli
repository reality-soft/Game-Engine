float4 WhiteColor()
{
    return float4(1, 1, 1, 1);
}

float4 CreateColor(Texture2D tex, SamplerState sample, float2 uv)
{
    float4 color = tex.Sample(sample, uv);
    if (length(color) == 0)
        return float4(1, 1, 1, 1);
    
    return color;
}