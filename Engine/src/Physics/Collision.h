#pragma once
#include "Shape.h"
#include "SimpleMath.h"

namespace reality {
    struct RayCallback
    {
        RayCallback()
        {
            success = false;
            is_actor = false;

            distance = 0.0f;
            point = XMVectorZero();
            normal = XMVectorZero();
        }
        bool success;
        bool is_actor;
        entt::entity ent;

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

    bool PointInTriangle(const XMVECTOR& p, const TriangleShape& tri);

    bool RayToAABB(const RayShape& ray, const AABBShape& aabb);

    RayCallback RayToTriangle(const RayShape& ray, const TriangleShape& tri);

    RayCallback RayToCapsule(const RayShape& ray, const CapsuleShape& cap);

    CollideType AABBtoAABB(const AABBShape& aabb1, const AABBShape& aabb2);

    CollideType CapsuleToAABB(const AABBShape& aabb, const CapsuleShape& capsule);

    CollideType SphereToAABB(const SphereShape& sphere, const AABBShape& aabb);

    CollideType CapsuleToCapsule(const CapsuleShape& cap1, const CapsuleShape& cap2);

    CollideType AABBToTriagnle(const AABBShape& aabb, const TriangleShape& triangle);

    CapsuleCallback CapsuleToTriangle(const CapsuleShape& cap, const TriangleShape& triangle);

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