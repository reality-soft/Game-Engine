#pragma once
#include "Components.h"
#include "Mesh.h"
#include "ConstantBuffer.h"

namespace reality
{
	class DLL_API SkySphere
	{
	public:
		SkySphere();
		~SkySphere();

	public:
		bool CreateSphere();
		void Update(float lerp_value);
		void Render();

		void SetNoonColor(XMFLOAT4 _color);
		void SetNightColor(XMFLOAT4 _color);
		XMFLOAT4 GetSkyColor();

	private:
		XMFLOAT4 skycolor_night;
		XMFLOAT4 skycolor_noon;
		CbSkySphere cb_sky;

	private:
		shared_ptr<StaticMesh> sphere_mesh;
		shared_ptr<VertexShader> vs;
	};
}
