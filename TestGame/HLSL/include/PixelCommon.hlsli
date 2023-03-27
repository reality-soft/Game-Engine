// Global Directional Lighting
cbuffer CbGlobalLight : register(b0)
{
    float4 position;
    float4 direction;
    float4 ambient;
}

// Point Lighting
struct PointLight
{
    float3  position;
    float   range;
    float3  attenuation;
    float   padding;
};

cbuffer CbPointLights : register(b1)
{
    PointLight point_lights[64];
}

// Spot Lighting
struct SpotLight
{
    float3  position;
    float   range;
    float3  attenuation;
    float   padding;
    float3  direction;
    float   spot;
};

cbuffer CbSpotLights : register(b2)
{
    SpotLight spot_lights[64];
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

float3 RGBtoHSV(float3 RGB)
{
    float3 HSV = 0;
    float M = min(RGB.r, min(RGB.g, RGB.b));
    HSV.z = max(RGB.r, max(RGB.g, RGB.b));
    float C = HSV.z - M;
    if (C != 0)
    {
        HSV.y = C / HSV.z;
        float3 D = (((HSV.z - RGB) / 6) + (C / 2)) / C;
        if (RGB.r == HSV.z)
            HSV.x = D.b - D.g;
        else if (RGB.g == HSV.z)
            HSV.x = (1.0 / 3.0) + D.r - D.b;
        else if (RGB.b == HSV.z)
            HSV.x = (2.0 / 3.0) + D.g - D.r;
        if (HSV.x < 0.0)
        {
            HSV.x += 1.0;
        }
        if (HSV.x > 1.0)
        {
            HSV.x -= 1.0;
        }
    }
    return HSV;
}

float3 HSVtoRGB(float3 HSV)
{
    float3 RGB = 0;
    float C = HSV.z * HSV.y;
    float H = HSV.x * 6;
    float X = C * (1 - abs(fmod(H, 2) - 1));
    if (HSV.y != 0)
    {
        float I = floor(H);
        if (I == 0)
        {
            RGB = float3(C, X, 0);
        }
        else if (I == 1)
        {
            RGB = float3(X, C, 0);
        }
        else if (I == 2)
        {
            RGB = float3(0, C, X);
        }
        else if (I == 3)
        {
            RGB = float3(0, X, C);
        }
        else if (I == 4)
        {
            RGB = float3(X, 0, C);
        }
        else
        {
            RGB = float3(C, 0, X);
        }
    }
    float M = HSV.z - C;
    return RGB + M;
}

float4 ChangeHue(float4 color, float amount)
{
    float3 hsv = RGBtoHSV(color.xyz);
    hsv.x *= amount;
    return float4(HSVtoRGB(hsv), 1.0f);
}

float4 ChangeSaturation(float4 color, float amount)
{
    float3 hsv = RGBtoHSV(color.xyz);
    hsv.y *= amount;
    return float4(HSVtoRGB(hsv), 1.0f);
}

float4 ChangeValue(float4 color, float amount)
{
    float3 hsv = RGBtoHSV(color.xyz);
    hsv.z *= amount;
    return float4(HSVtoRGB(hsv), 1.0f);
}

float4 ApplyDirectionalLight(float4 color, float3 normal)
{
    float intensity = saturate(dot(normal, -direction.xyz));
    return saturate(color * intensity);
}

float4 ApplyAmbientLight(float4 color)
{
    
    return max(color, ambient);
}

float4 ApplySpecularLight(float4 color, float3 view_dir, float3 reflection, float power)
{
    float specular = pow(saturate(dot(view_dir, reflection)), power);
    
    color = ChangeSaturation(color, 1.0f + (specular * 10));
    color = ChangeValue(color, 1.0f + (specular * 10));
    
    return saturate(color + specular);
}

float4 ApplyPointLights(float4 color, float3 origin, float3 normal)
{
    return float4(0, 0, 0, 1);
}

float4 ApplySpotLights(float4 color, float3 normal)
{
    return float4(0, 0, 0, 1);
}