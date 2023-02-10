#pragma once
#include "DX11App.h"

namespace KGCA41B
{


	class DLL_API VertexShader
	{
	public:
		virtual bool LoadCompiled(wstring cso_file);
		ID3D11VertexShader* Get();
		ID3D11InputLayout* InputLayoyt();
	protected:
		ComPtr<ID3D11VertexShader> vs;
		ComPtr<ID3D11InputLayout> input_layout;
	};

	class DLL_API PixelShader
	{
	public:
		bool LoadCompiled(wstring _csoFile);
		ID3D11PixelShader* Get();
	private:
		ComPtr<ID3D11PixelShader> ps;
	};
}

