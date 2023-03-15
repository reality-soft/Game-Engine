#pragma once
#include "Shape.h"

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

    static RayCallback RayToTriangle(RayShape& ray, TriangleShape& tri)
    {
        RayCallback callback;
        XMVECTOR P = XMPlaneIntersectLine(XMPlaneFromPoints(tri.vertex0, tri.vertex1, tri.vertex2), ray.start, ray.end);

        if (tri.SameSide(P, tri.vertex0, tri.vertex1, tri.vertex2) &&
            tri.SameSide(P, tri.vertex1, tri.vertex0, tri.vertex2) &&
            tri.SameSide(P, tri.vertex2, tri.vertex0, tri.vertex1))
        {
            float distance = XMVectorGetX(XMVector3Length(P - ray.start));
            if (distance <= XMVectorGetX(XMVector3Length(ray.end - ray.start)))
            {
                callback.success = true;
                callback.distance = distance;
                callback.point = P;
                callback.normal = tri.normal;
            }
        }


        return callback;
    }

    static bool RayToAABB(RayShape& ray, AABBShape& aabb)
    {
        for (int i = 0; i < 12; ++i)
        {
            auto& callback = RayToTriangle(ray, aabb.triangle[i]);
            if (callback.success)
                return true;
        }

        return false;
    }

    static CollideType FrustumToAABB(Frustum& frustum, AABBShape& aabb)
    {
        int in_axies = 0;
        int out_axies = 0;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                bool success = RayToTriangle(aabb.vertical_ray[i], frustum.topbottom_tries[j]).success;

                if (success)
                    in_axies++;
                else
                    out_axies++;
            }
        }

        if (in_axies == 0)
        {
            return CollideType::OUTSIDE;
        }

        if (out_axies == 0)
        {
            return CollideType::INSIDE;
        }
        return CollideType::INTERSECT;

        //int in_corner = 0;
        //int out_corner = 0;

        //XMVECTOR corners[8];
        //corners[0] = XMVectorSet(XMVectorGetX(aabb.min), XMVectorGetY(aabb.min), XMVectorGetZ(aabb.min), 1);
        //corners[1] = XMVectorSet(XMVectorGetX(aabb.min), XMVectorGetY(aabb.min), XMVectorGetZ(aabb.max), 1);
        //corners[2] = XMVectorSet(XMVectorGetX(aabb.min), XMVectorGetY(aabb.max), XMVectorGetZ(aabb.min), 1);
        //corners[3] = XMVectorSet(XMVectorGetX(aabb.min), XMVectorGetY(aabb.max), XMVectorGetZ(aabb.max), 1);
        //corners[4] = XMVectorSet(XMVectorGetX(aabb.max), XMVectorGetY(aabb.min), XMVectorGetZ(aabb.min), 1);
        //corners[5] = XMVectorSet(XMVectorGetX(aabb.max), XMVectorGetY(aabb.min), XMVectorGetZ(aabb.max), 1);
        //corners[6] = XMVectorSet(XMVectorGetX(aabb.max), XMVectorGetY(aabb.max), XMVectorGetZ(aabb.min), 1);
        //corners[7] = XMVectorSet(XMVectorGetX(aabb.max), XMVectorGetY(aabb.max), XMVectorGetZ(aabb.max), 1);

        //for (int i = 0; i < 6; ++i)
        //{
        //    float dot = 0;
        //    for (int j = 0; j < 8; ++j)
        //    {
        //        dot = frustum.frustum_plane[i].DotFromPoint(corners[j]);

        //        if (dot < 0) out_corner++;
        //        else in_corner++;
        //    }

        //    if (in_corner == 0)
        //    {
        //        return CollideType::OUTSIDE;
        //    }

        //    if (out_corner == 0)
        //    {
        //        return CollideType::INSIDE;
        //    }

        //}
        //return CollideType::INTERSECT;
    }

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

        //if (triangle.RayIntersection(RayShape(cap_ab[0], cap_ab[1])).first == true)
        //    return CollideType::INSIDE;

        return CollideType::OUTSIDE;
    }
}