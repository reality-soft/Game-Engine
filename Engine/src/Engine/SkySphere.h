#pragma once
#include "DllMacro.h"
#include "DataTypes.h"
#include "Components.h"

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

		XMFLOAT4 skycolor_afternoon;
		XMFLOAT4 skycolor_noon;
		XMFLOAT4 skycolor_night;
		FLOAT color_strength;
		CbSkySphere cb_sky;

	private:
		void FrameBackgroundSky(const C_Camera* camera);
		void RenderBackgroundSky();

		void FrameSunSky(const C_Camera* camera);
		void RenderSunSky();

		void FrameStarSky(const C_Camera* camera);
		void RenderStarSky();

		void FrameCloudSky(const C_Camera* camera);
		void RenderCloudSky();

	private:
		shared_ptr<StaticMesh> sphere_mesh;
		shared_ptr<StaticMesh> cloud_dome;
		shared_ptr<VertexShader> vs;
		CbTransform cb_transform;
	};
}
