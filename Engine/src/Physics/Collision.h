#pragma once
#include "Shape.h"
#include "SimpleMath.h"

namespace reality {
    struct RayCallback
    {
        RayCallback()
        {
            success = false;
            distance = 0.0f;
            point = XMVectorZero();
            normal = XMVectorZero();
        }
        bool success;
        float distance;
        XMVECTOR point;
        XMVECTOR normal;
    };

	enum class CollideType
	{
		OUTSIDE,
		INTERSECT,
		INSIDE,
	};

    struct CapsuleCallback
    {
        CapsuleCallback()
        {
            reaction = NONE;
            floor_pos = XMVectorZero();
        }
        enum
        {
            FLOOR,
            WALL,
            NONE
        } reaction;
        XMVECTOR floor_pos;

    };

    // CollisionTest

    bool SameSide(XMVECTOR p1, XMVECTOR p2, XMVECTOR a, XMVECTOR b);

    bool PointInTriangle(const XMVECTOR& p, TriangleShape& tri);

    bool RayToAABB(const RayShape& ray, AABBShape& aabb);

    RayCallback RayToTriangle(RayShape& ray, TriangleShape& tri);

    RayCallback RayToCapsule(RayShape& ray, const CapsuleShape& cap);

    CollideType AABBtoAABB(AABBShape& aabb1, AABBShape& aabb2);

    CollideType CapsuleToAABB(AABBShape& aabb, CapsuleShape& capsule);

    CollideType SphereToAABB(const SphereShape& sphere, const AABBShape& aabb);

    CollideType CapsuleToCapsule(CapsuleShape& cap1, CapsuleShape& cap2);

    CollideType AABBToTriagnle(AABBShape& aabb, TriangleShape& triangle);

    CapsuleCallback CapsuleToTriangle(CapsuleShape& cap, TriangleShape& triangle);

    // Converter

    XMVECTOR GetRayVector(const RayShape& ray);

    array<RayShape, 3> GetEdgeRays(const TriangleShape& tri);
    XMVECTOR           GetMinXZ(const TriangleShape& tri);
    XMVECTOR           GetMaxXZ(const TriangleShape& tri);

    array<TriangleShape, 12> GetTriangles(const AABBShape& aabb);
    array<XMFLOAT3, 8>       GetCorners(const AABBShape& aabb);
    array<RayShape, 4>       GetYAxisRay(const AABBShape& aabb);

    array<XMVECTOR, 4> GetTipBaseAB(const CapsuleShape& capsule);


    AABBShape   CapsuleConvertAABB(const CapsuleShape& capsule);
    SphereShape AABBTConvertSphere(const AABBShape& aabb);
}