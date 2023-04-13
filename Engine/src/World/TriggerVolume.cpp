#include "stdafx.h"
#include "TriggerVolume.h"

void reality::TriggerVolume::Create(const AABBShape& aabb)
{
	volume_aabb.CreateFromPoints(volume_aabb, _XMVECTOR3(aabb.min), _XMVECTOR3(aabb.max));
	volume_sphere.CreateFromBoundingBox(volume_sphere, volume_aabb);
}

void reality::TriggerVolume::Create(const XMFLOAT3& offset, const float radius)
{
	volume_sphere = BoundingSphere(offset, radius);
	volume_aabb.CreateFromSphere(volume_aabb, volume_sphere);
}

bool reality::TriggerVolume::IsCapsuleOverlapBox(const CapsuleShape& capsule)
{
	auto capsule_info = GetTipBaseAB(capsule);
	XMVECTOR closest_point = PointLineSegment(_XMVECTOR3(volume_aabb.Center), capsule_info[2], capsule_info[3]);
	
	if (volume_aabb.Intersects(BoundingSphere(_XMFLOAT3(closest_point), capsule.radius)))
		return true;

	return false;
}
