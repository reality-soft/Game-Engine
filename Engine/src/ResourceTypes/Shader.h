#pragma once
#include "DX11App.h"

namespace reality
{
	class DLL_API VertexShader
	{
	public:
		virtual bool LoadCompiled(wstring cso_file);
		ID3D11VertexShader* Get();
		ID3D11InputLayout* InputLayout();
	protected:
		ComPtr<ID3D11VertexShader> vs;
		ComPtr<ID3D11InputLayout> input_layout;
	};

	class DLL_API PixelShader
	{
	public:
		bool LoadCompiled(wstring cso_file);
		ID3D11PixelShader* Get();
	private:
		ComPtr<ID3D11PixelShader> ps;
	};

	class DLL_API GeometryShader
	{
	public:
		bool LoadCompiled(wstring cso_file);
		ID3D11GeometryShader* GetStreamOutGS();
		ID3D11GeometryShader* GetDefaultGS();
	private:
		ComPtr<ID3D11GeometryShader> stream_out_gs;
		ComPtr<ID3D11GeometryShader> default_gs;
	};

	class DLL_API ComputeShader
	{
	public:
		bool LoadCompiled(wstring cso_file);
		ID3D11ComputeShader* Get();
	private:
		ComPtr<ID3D11ComputeShader> cs;
	};
}

