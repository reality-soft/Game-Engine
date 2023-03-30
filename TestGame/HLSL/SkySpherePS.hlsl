#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float4 n : NORMAL;
    float2 t : TEXCOORD;
    float3 origin : TEXCOORD1;
};

cbuffer CbSkySphere : register(b1)
{
    float4 sky_color;
    float4 gradation;
}

float4 PS(PS_OUT output) : SV_Target  
{      
    float4 skycolor = sky_color;
    
    return skycolor;
    //if (output.origin.y >= 0)
    //{        
    //    skycolor.x -= pow(output.origin.y, strength.w) * 0.5;
    //    skycolor.y -= pow(output.origin.y, strength.w) * 0.5;
    //    skycolor.z -= pow(output.origin.y, strength.w) * 0.5;
    //}
    
    //return ApplyDistanceFog(skycolor, output.origin);
    //return skycolor;
}