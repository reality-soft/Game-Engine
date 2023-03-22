#include "include/EffectCommonHeaderGS.hlsli"

[maxvertexcount(4)]
void GS(
	in point GS_IN input[1] : SV_POSITION,
	inout TriangleStream< GS_OUT > output)
{
	// TriangleStrip으로 구성한다
	// p 설정
	const float3 g_positions[4] =
	{
		float3(-1.0, +1.0, 0.0),
		float3(+1.0, +1.0, 0.0),
		float3(-1.0, -1.0, 0.0),
		float3(+1.0, -1.0, 0.0),
	};

	// t 설정
	const float2 g_texcoords[4] =
	{
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
	};

	int frame = max((int)(g_emitter_values.z * g_particle_values.y) - 1, 0);
	
	const float2 g_uvtexcoords[4] =
	{
		float2(g_emitter_values2[frame].x, g_emitter_values2[frame].y),
		float2(g_emitter_values2[frame].z, g_emitter_values2[frame].y),
		float2(g_emitter_values2[frame].x, g_emitter_values2[frame].w),
		float2(g_emitter_values2[frame].z, g_emitter_values2[frame].w),
	};

	GS_OUT vertex;
	for (int i = 0; i < 4; i++)
	{
		float4 new_point = float4(input[0].p + float4(g_positions[i].xyz, 1.0f));
		//float4 local = mul(new_point, g_mat_particle);
		//float4 world = mul(local, g_world);
		//world.y += -9.81f * g_particle_values.x * g_particle_values.x;
		//float4 bill = mul(world, g_billboard);
		//float4 view = mul(bill, g_view);

		float4x4 particle_world_matrix = mul(g_mat_particle, g_world);
		if(g_emitter_values.w)
			particle_world_matrix[3][1] += -9.81f * g_particle_values.x * g_particle_values.x;
		float x_offset = particle_world_matrix[3][0];
		float y_offset = particle_world_matrix[3][1];
		float z_offset = particle_world_matrix[3][2];
		particle_world_matrix[3][0] = 0;
		particle_world_matrix[3][1] = 0;
		particle_world_matrix[3][2] = 0;
		float4x4 final_matrix = mul(particle_world_matrix, g_billboard);
		final_matrix[3][0] += x_offset;
		final_matrix[3][1] += y_offset;
		final_matrix[3][2] += z_offset;
		float4 world = mul(new_point, final_matrix);
		float4 view = mul(world, g_view);
		float4 proj = mul(view, g_proj);


		//float4 local = mul(new_point, g_mat_particle);
		//float4 world = mul(local, g_world);
		//world.y += -9.81f * g_particle_values.x * g_particle_values.x;
		//float4 bill = mul(world, g_billboard);
		//float4 view = mul(bill, g_view);
		//float4 proj = mul(view, g_proj);
		vertex.p = proj;

		vertex.c = input[0].c * g_color;
		vertex.t = g_texcoords[i];
		vertex.uv = g_uvtexcoords[i];

		output.Append(vertex);
	}
	output.RestartStrip();
}