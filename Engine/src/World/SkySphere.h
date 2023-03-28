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
		void FrameRender(const C_Camera* camera);

		XMMATRIX sun_world;

		XMFLOAT4 skycolor_afternoon;
		XMFLOAT4 skycolor_noon;
		XMFLOAT4 skycolor_night;
		FLOAT color_strength;
		CbSkySphere cb_sky;

	private:
		void FrameBackgroundSky(const C_Camera* camera);
		void RenderBackgroundSky();

	private:
		shared_ptr<StaticMesh> sphere_mesh;
		shared_ptr<VertexShader> vs;
		CbTransform cb_transform;
	};
}
