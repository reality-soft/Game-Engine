#pragma once
#include "DX11App.h"

namespace KGCA41B
{
	class DLL_API VsDefault
	{
	public:
		virtual bool LoadCompiled(wstring cso_file);
		ID3D11VertexShader* Get();
	protected:
		ComPtr<ID3D11VertexShader> vs;
		ComPtr<ID3D11InputLayout> input_layout;
	};

	class DLL_API VsSkinned : public VsDefault
	{
	public:
		bool LoadCompiled(wstring cso_file) override;
	};

	class DLL_API PsDefault
	{
	public:
		bool LoadCompiled(wstring _csoFile);
		ID3D11PixelShader* Get();
	private:
		ComPtr<ID3D11PixelShader> ps;
	};
}

