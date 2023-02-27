#include "include/VertexCommon.hlsli"
struct VS_IN
{
    float3 p : F3_POSITION;
    float3 n : F3_NORMAL;
    float4 c : F4_COLOR;
    float2 t : F2_TEXTURE;
    float4 i : F4_INDEX;
    float4 w : F4_WEIGHT;
};
struct VS_OUT
{
    float4 p : SV_POSITION;
    float4 n : NORMAL;
    float4 c : COLOR0;
    float2 t : TEXCOORD0;
};

cbuffer cb_data : register(b1)
{
    matrix world_matrix;
};

cbuffer cb_bone_data : register(b2)
{
    matrix anim_matrix[255];
}

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    float4 local = float4(input.p, 0.0f);
    float4 normal = float4(input.n, 0.0f);
    
    float4 animation = 0;
    float4 anim_normal = 0;

    for (int s = 0; s < 4; s++)
    {
        uint sk_index = input.i[s];
        float weight = input.w[s];

        animation += mul(local, anim_matrix[sk_index]) * weight;
        anim_normal += mul(normal, anim_matrix[sk_index]) * weight;
    }

    float4 world = mul(animation, world_matrix);
    float4 projection = mul(world, ViewProjection());

    output.p = projection;
    output.n = anim_normal;
    output.c = input.c;
    output.t = input.t;

    return output;
}