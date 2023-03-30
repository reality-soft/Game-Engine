#include "include/VertexCommon.hlsli"

struct VS_IN
{
    float3 p : F3_POSITION;  
    float3 n : F3_NORMAL;
    float2 t : F2_TEXTURE;
};
struct VS_OUT  
{
    float4 p : SV_POSITION;
    float4 n : NORMAL;
    float2 t : TEXCOORD;
    float3 origin : TEXCOORD1;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    float4 local = float4(input.p, 1.0f);
    float4 normal = float4(input.n, 1.0f);
    
    float4 world = mul(local, FarScaleMatrix() * camera_translation);
    float4 projection = mul(world, ViewProjection());

    output.origin = world;
    
    output.p = projection;
    output.n = normal;
    output.t = input.t;

    return output;
}