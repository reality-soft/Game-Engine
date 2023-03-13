#pragma once
#include "Shape.h"

namespace reality {
	enum class CollideType
	{
		OUTSIDE,
		INTERSECT,
		INSIDE,
	};

	static CollideType AABBtoAABB(AABBShape& aabb1, AABBShape& aabb2)
	{
        for (int i = 0; i < 3; i++)
        {
            bool overlap = 
                aabb1.max.m128_f32[i] >= aabb2.min.m128_f32[i]
                &&
                aabb1.min.m128_f32[i] <= aabb2.max.m128_f32[i];

            if (!overlap)
                return CollideType::OUTSIDE;
        }

        bool aabb1_inside = DirectX::XMVector3GreaterOrEqual(aabb1.max, aabb2.max) && DirectX::XMVector3LessOrEqual(aabb1.min, aabb2.max);
        bool aabb2_inside = DirectX::XMVector3GreaterOrEqual(aabb2.max, aabb1.max) && DirectX::XMVector3LessOrEqual(aabb2.max, aabb1.min);

        if (aabb1_inside || aabb2_inside)
            return CollideType::INSIDE;

        return CollideType::INTERSECT;
	}

    static CollideType CapsuleToCapsule(CapsuleShape& cap1, CapsuleShape& cap2)
    {
        vector<XMVECTOR> cap1_ab = cap1.GetAB();
        vector<XMVECTOR> cap2_ab = cap2.GetAB();

        float distance = 0;
        float radius = cap1.radius + cap2.radius;

        // cap1 to cap2_A
        distance = XMVectorGetX(XMVector3LinePointDistance(cap1_ab[0], cap1_ab[1], cap2_ab[0]));
        if (distance < radius)
            return CollideType::INTERSECT;
        // cap1 to cap2_B
        distance = XMVectorGetX(XMVector3LinePointDistance(cap1_ab[0], cap1_ab[1], cap2_ab[1]));
        if (distance < radius)
            return CollideType::INTERSECT;

        // cap2 to cap1_A
        distance = XMVectorGetX(XMVector3LinePointDistance(cap2_ab[0], cap2_ab[1], cap1_ab[0]));
        if (distance < radius)
            return CollideType::INTERSECT;
        // cap2 to cap1_B
        distance = XMVectorGetX(XMVector3LinePointDistance(cap2_ab[0], cap2_ab[1], cap1_ab[1]));
        if (distance < radius)
            return CollideType::INTERSECT;

        return CollideType::OUTSIDE;
    }

    static CollideType AABBToTriagnle(AABBShape& aabb, TriangleShape& triangle)
    {
        // Check if all three vertices are inside the AABB
        int tri_vertices = 0;

        if (XMVector3GreaterOrEqual(triangle.vertex0, aabb.min) && XMVector3LessOrEqual(triangle.vertex0, aabb.max))
            tri_vertices++;

        if (XMVector3GreaterOrEqual(triangle.vertex1, aabb.min) && XMVector3LessOrEqual(triangle.vertex1, aabb.max))
            tri_vertices++;

        if (XMVector3GreaterOrEqual(triangle.vertex2, aabb.min) && XMVector3LessOrEqual(triangle.vertex2, aabb.max))
            tri_vertices++;

        if (tri_vertices == 3)
            return CollideType::INSIDE;

        if (tri_vertices > 0)
            return CollideType::INTERSECT;


        return CollideType::OUTSIDE;
    }

    static CollideType CapsuleToTriangle(CapsuleShape& cap, TriangleShape& triangle)
    {
        vector<XMVECTOR> cap_ab = cap.GetAB();

        float distance = 0;
        float radius = cap.radius;

        // cap1 to cap2_A
        distance = XMVectorGetX(XMVector3LinePointDistance(cap_ab[0], cap_ab[1], triangle.vertex0));
        if (distance < radius)
            return CollideType::INTERSECT;

        distance = XMVectorGetX(XMVector3LinePointDistance(cap_ab[0], cap_ab[1], triangle.vertex1));
        if (distance < radius)
            return CollideType::INTERSECT;

        distance = XMVectorGetX(XMVector3LinePointDistance(cap_ab[0], cap_ab[1], triangle.vertex2));
        if (distance < radius)
            return CollideType::INTERSECT;

        if (triangle.RayIntersection(RayShape(cap_ab[0], cap_ab[1])).first == true)
            return CollideType::INSIDE;

        return CollideType::OUTSIDE;
    }
}