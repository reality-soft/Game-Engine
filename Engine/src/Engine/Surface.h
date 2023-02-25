#pragma once
#include "DataTypes.h"
#include "DllMacro.h"
#include "Shader.h"

namespace KGCA41B
{
	struct Material
	{
		Material()
		{
			texture_id.resize(7);
		}
		vector<string> texture_id;
	};

	class DLL_API Surface
	{
	public:
		string shader_id = "SurfacePS.cso";
		vector<Material> materials;

		vector<ID3D11ShaderResourceView*> texture_array_s0;
	public:
		void CreateSurface();
		void SetSurface();

		void SaveDefinition(string filename);
		void LoadDefinition(string filename);

	private:
		ComPtr<ID3D11Buffer> srv_buffer;
		shared_ptr<PixelShader> pixel_shader;
	};
}
