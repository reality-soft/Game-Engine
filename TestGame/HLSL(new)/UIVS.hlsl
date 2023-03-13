cbuffer cb_viewproj : register(b0)
{
    matrix g_mat_view;
    matrix g_mat_proj;
}

cbuffer cb_data : register(b1)
{
    matrix world_matrix;
};

struct VS_IN
{
    float2 p : F2_POSITION;
    float2 t : F2_TEXTURE;
};

struct VS_OUT
{
    float4 p : SV_POSITION;
    float2 t : TEXCOORD0;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    float4 local = float4(input.p, 1.0f, 1.0f);
    
    float4 world = mul(local, world_matrix);

    output.p = world;
    output.t = input.t;

    return output;
}