#pragma once
#include "Shader.h"

namespace reality
{
	class DLL_API Material
	{
	public:
		Material();
		~Material() = default;

	public:
		string diffuse;
		string normalmap;
		string metalic;
		string roughness;
		string specular;
		string ambient;
		string opacity;
		string shader = "MeshSurfacePS.cso";

		ID3D11ShaderResourceView* textures[7];
		ID3D11PixelShader* pixel_shader;
		ID3D11SamplerState* sampler;

		void SaveEmpty(string filename);
		void Create();
		void Save(string filename);
		void Set();
		void LoadAndCreate(string filename);
	};
}
