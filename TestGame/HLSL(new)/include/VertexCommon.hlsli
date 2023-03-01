// Camera View-Proj
cbuffer cb_data : register(b0)
{
    matrix view_matrix;
    matrix projection_matrix;
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