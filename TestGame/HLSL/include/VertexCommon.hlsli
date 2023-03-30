// Camera Info
cbuffer CbCameraInfo : register(b0)
{
    matrix view_matrix;
    matrix projection_matrix;
    matrix camera_translation;
    float4 camera_world;
    float4 camera_look;
};

// Returns View X Projection
float4x4 ViewProjection()
{
    return mul(view_matrix, projection_matrix);
}

// Identity Matrix
float4x4 IdentityMatrix()
{
    return float4x4(  
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,  
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

float4x4 FarScaleMatrix()
{
    return float4x4(
        camera_world.w * 0.9, 0.0, 0.0, 0.0,
        0.0, camera_world.w * 0.9, 0.0, 0.0,
        0.0, 0.0, camera_world.w * 0.9, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

float GetLod(float3 vertex)
{
    float distance = length(vertex - camera_world.xyz);
    return (distance / 40) + 4;
}