#pragma once
#include "DataTypes.h"
#include "DllMacro.h"
#include "Shader.h"

namespace KGCA41B
{
	class Material
	{
	public:
		Material() = default;
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

		void CreateAndSave(string filename);
		void LoadAndCreate(string filename);
	};
}
