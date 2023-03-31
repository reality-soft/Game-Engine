struct PS_IN
{
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD0;
};


float4 PS(PS_IN input) : SV_Target
{
    float4 depth_color = { input.p.z, 0, 0, 1 };
    
    return depth_color;    
}