struct GS_IN
{
    float4 p : SV_POSITION;
    float2 t : TEXCOORD;
    float lod : TEXCOORD1;
    
    float3 view_dir : TEXCOORD2;
    matrix view_proj : TEXCOORD3;
};

struct GS_OUT
{
    float4 p : SV_POSITION;
    float2 t : TEXCOORD;    
    float lod : TEXCOORD1;
    
    float3 view_dir : TEXCOORD2;    
    float3 normal : NORMAL0;
    float3 origin : NORMAL1;
    //float3 tangent;
    //float3 binormal;
};

[maxvertexcount(3)]
void GS(triangle GS_IN input[3], inout TriangleStream<GS_OUT> stream)
{
    GS_OUT gs_stream = (GS_OUT) 0;
	  
    float3 edge1 = input[2].p - input[0].p;
    float3 edge2 = input[1].p - input[0].p;
	
    for (int i = 0; i < 3; ++i)
    {
        gs_stream.p = mul(input[i].p, input[i].view_proj);
        gs_stream.t = input[i].t;   
        gs_stream.lod = input[i].lod;
        
        gs_stream.view_dir = input[i].view_dir;
        gs_stream.normal = normalize(cross(edge1, edge2));
        gs_stream.origin = input[i].p;

		
        stream.Append(gs_stream);
    }
    stream.RestartStrip();
}