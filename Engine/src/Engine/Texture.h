#pragma once
#include "common.h"
#include "DllMacro.h"

#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

namespace KGCA41B
{
	class DLL_API Texture
	{
	public:
		Texture() = default;
		~Texture() = default;

	public:
		bool LoadTextureWIC(wstring filepath);

		vector<ID3D11Resource*> res_list;
		vector<ID3D11ShaderResourceView*> srv_list;
	};
}