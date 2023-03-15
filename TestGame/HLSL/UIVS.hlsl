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
    
    float4 local = float4(input.p, 0.0f, 1.0f);
    
    matrix new_world = world_matrix;
    new_world[3][0] = new_world[3][0] * 2 - 1;
    new_world[3][1] = -(new_world[3][1] * 2 - 1);

    float4 world = mul(local, new_world);

    //world.x = world.x * 2 - 1;
    //world.y = -(world.y * 2 - 1);

    output.p = world;
    output.t = input.t;

    return output;
}