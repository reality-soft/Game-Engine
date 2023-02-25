#pragma once
#include "DataTypes.h"
#include "DllMacro.h"
#include "Shader.h"

namespace KGCA41B
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
		string sepcular;
		string ambient;
		string opacity;
		string shader = "SurfacePS.cso";

		ID3D11ShaderResourceView* textures[7];
		ID3D11PixelShader* pixel_shader;

		void SaveEmpty(string filename);
		void Create();
		void Save(string filename);
		void Set();
		void LoadAndCreate(string filename);
	};
}
