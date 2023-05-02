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
        UINT tri_index;

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

    bool DLL_API SameSide(XMVECTOR p1, XMVECTOR p2, XMVECTOR a, XMVECTOR b);

    bool DLL_API PointInTriangle(const XMVECTOR& p, const TriangleShape& tri);

    bool DLL_API RayToAABB(const RayShape& ray, const AABBShape& aabb);

    RayCallback DLL_API RayToTriangle(const RayShape& ray, const TriangleShape& tri);

    RayCallback DLL_API RayToCapsule(const RayShape& ray, const CapsuleShape& cap);

    CollideType DLL_API AABBtoAABB(const AABBShape& aabb1, const AABBShape& aabb2);

    CollideType DLL_API CapsuleToAABB(const AABBShape& aabb, const CapsuleShape& capsule);

    CollideType DLL_API SphereToAABB(const SphereShape& sphere, const AABBShape& aabb);

    CollideType DLL_API CapsuleToCapsule(const CapsuleShape& cap1, const CapsuleShape& cap2);

    CollideType DLL_API CapsuleToSphere(const CapsuleShape& cap, const SphereShape& sphere);

    CollideType DLL_API AABBToTriagnle(const AABBShape& aabb, const TriangleShape& triangle);

    CapsuleCallback DLL_API CapsuleToTriangle(const CapsuleShape& cap, const TriangleShape& triangle);

    XMVECTOR DLL_API PointRaySegment(const RayShape& ray, const XMVECTOR& point);

    // Converter

    XMVECTOR DLL_API GetRayDirection(const RayShape& ray);

    array<RayShape, 3> DLL_API GetEdgeRays(const TriangleShape& tri);
    XMVECTOR           DLL_API GetMinXZ(const TriangleShape& tri);
    XMVECTOR           DLL_API GetMaxXZ(const TriangleShape& tri);

    array<TriangleShape, 12> DLL_API GetTriangles(const AABBShape& aabb);
    array<XMFLOAT3, 8>       DLL_API GetCorners(const AABBShape& aabb);
    array<RayShape, 4>       DLL_API GetYAxisRay(const AABBShape& aabb);

    array<XMVECTOR, 4> DLL_API GetTipBaseAB(const CapsuleShape& capsule);


    AABBShape   DLL_API CapsuleConvertAABB(const CapsuleShape& capsule);
    SphereShape DLL_API AABBTConvertSphere(const AABBShape& aabb);
}