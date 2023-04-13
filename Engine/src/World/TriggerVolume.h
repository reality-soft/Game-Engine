#pragma once
#include "Collision.h"

namespace reality
{
	class DLL_API TriggerVolume
	{
	public:
		void Create(const AABBShape& aabb);
		void Create(const XMFLOAT3& offset, const float radius);

	public:
		BoundingBox volume_aabb;
		BoundingSphere volume_sphere;

	public:
		bool IsCapsuleOverlapBox(const CapsuleShape& capsule);
	};

}
