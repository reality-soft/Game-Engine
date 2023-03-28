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
    float3 diffuse;
    float pad1;
    float3 specular;
    float pad2;
    float3 ambient;
    float pad3;

    float3 position;
    float range;
    float3 attenuation;
    float pad4;
};

cbuffer CbPointLights : register(b1)
{
    PointLight point_lights[64];
}

// Spot Lighting
struct SpotLight
{
    float3 diffuse;
    float pad1;
    float3 specular;
    float pad2;
    float3 ambient;
    float pad3;

    float3 position;
    float range;
    float3 attenuation;
    float pad4;
    float3 direction;
    float spot;
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

float4 ApplyCookTorrance(float4 diffuse, float roughness, float specular, float3 normal, float3 view_dir)
{    
    // Correct the input and compute aliases
    view_dir = normalize(view_dir);
    float3 light_dir = normalize(-direction);
    float3 half_vec = normalize(light_dir + view_dir);
    float normal_dot_half = dot(normal, half_vec);
    float view_dot_half = dot(half_vec, view_dir);
    float normal_dot_view = dot(normal, view_dir);
    float normal_dot_light = dot(normal, light_dir);
    
    // Compute the geometric term  
    float G1 = (2.0f * normal_dot_half * normal_dot_view) / view_dot_half;
    float G2 = (2.0f * normal_dot_half * normal_dot_light) / view_dot_half;
    float G = min(1.0f, max(0.0f, min(G1, G2)));
    
    // Compute the fresnel term
    float F = roughness + (1.0f - roughness) * pow(1.0f - normal_dot_view, 5.0f);
    
    // Compute the roughness term  
    float R_2 = roughness * roughness;
    float NDotH_2 = normal_dot_half * normal_dot_half;
    float A = 1.0f / (4.0f * R_2 * NDotH_2 * NDotH_2);
    float B = exp(-(1.0f - NDotH_2) / (R_2 * NDotH_2));
    float R = A * B;
    
    // Compute the final term  
    float3 S = specular * ((G * F * R) / (normal_dot_light * normal_dot_view));
    float3 flinal_color = WhiteColor().xyz * max(0.2f, normal_dot_light) * (diffuse.xyz + S);
    return float4(flinal_color, 1.0f);
}

float4 ApplyPointLights(float4 color, float3 origin, float3 normal)
{
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 spec = float3(0.0f, 0.0f, 0.0f);
    float3 ambient = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 64; i++)
    {
        if (point_lights[i].range == 0)
            continue;

        float3 light_vector = point_lights[i].position - origin;

        float d = length(light_vector);

        if (d > point_lights[i].range)
            continue;

        float3 D = float3(0.0f, 0.0f, 0.0f);
        float3 S = float3(0.0f, 0.0f, 0.0f);

        light_vector /= d;

        float diffuse_factor = dot(light_vector, normal);

        if (diffuse_factor > 0.0f)
        {
            float3 v = reflect(-light_vector, normal);

            float spec_factor;

            D = diffuse_factor * point_lights[i].diffuse;
            S = spec_factor * point_lights[i].diffuse;
        }

        float att = 1.0f / dot(point_lights[i].attenuation, float3(1.0f, d, d * d));

        D = att;
        S = att;

        diffuse += D;
        spec += S;
        ambient += point_lights[i].ambient;
    }

    return color * float4(diffuse + spec + ambient, 1.0f);
}

float4 ApplySpotLights(float4 color, float3 normal)
{
    return float4(0, 0, 0, 1);
}