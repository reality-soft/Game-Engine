#pragma once
#include "ConstantBuffer.h"

namespace reality
{
	class DLL_API DistanceFog
	{
	public:
		bool CreateDistacneFog();
		void SetMaxMinDistance(float max, float min);

		void UpdateFogStart(XMVECTOR fog_start);
		void UpdateFogColor(XMFLOAT4 color);

		void Update(XMFLOAT2 world_time, float current_time);
		void Render();

	private:
		CbFog cb_fog_;
		float max_distance_;
		float min_distance_;
	};
}
