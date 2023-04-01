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

    static bool PointInTriangle(const XMVECTOR& p, const TriangleShape& tri)
    {
        if (tri.SameSide(p, tri.vertex0, tri.vertex1, tri.vertex2) &&
            tri.SameSide(p, tri.vertex1, tri.vertex0, tri.vertex2) &&
            tri.SameSide(p, tri.vertex2, tri.vertex0, tri.vertex1))
        {
            XMVECTOR cross = XMVector3Cross(tri.vertex0 - p, tri.vertex1 - p);
            if (IsParallelVector(cross, tri.normal))
                return true;
        }
        return false;
    }

    static RayCallback RayToTriangle(const RayShape& ray, const TriangleShape& tri)
    {
        RayCallback callback;
        XMVECTOR P = XMPlaneIntersectLine(XMPlaneFromPoints(tri.vertex0, tri.vertex1, tri.vertex2), ray.start, ray.end);

        if (PointInTriangle(P, tri))
        {
            float distance = Distance(P, ray.start);
            float ray_length = Distance(ray.end, ray.start);
            if (distance <= ray_length)
            {
                callback.success = true;
                callback.distance = distance;
                callback.point = P;
                callback.normal = tri.normal;
            }
        }


        return callback;
    }

    static bool RayToAABB(const RayShape& ray, AABBShape& aabb)
    {
        XMVECTOR center_to_corner = aabb.max - aabb.center;
        center_to_corner.m128_f32[1] = 0.0f;
        float box_radius = XMVectorGetX(XMVector3Length(center_to_corner));

        XMVECTOR line1 = ray.start; line1.m128_f32[1] = aabb.center.m128_f32[1];
        XMVECTOR line2 = ray.end; line2.m128_f32[1] = aabb.center.m128_f32[1];
        float box_to_line = XMVectorGetX(XMVector3LinePointDistance(line1, line2, aabb.center));

        if (box_to_line <= box_radius)
            return true;

        //for (int i = 0; i < 12; ++i)
        //{
        //    auto& callback = RayToTriangle(ray, aabb.triangle[i]);
        //    if (callback.success)
        //        return true;
        //}

        return false;
    }

    static RayCallback RayToCapsule(const RayShape& ray, const CapsuleShape& cap)
    {
        XMVECTOR RC;
        float d;
        float t, s;
        XMVECTOR n, D, O;
        float ln;
        float in, out;

        XMVECTOR axis = XMVectorSet(0.0f, 1.0f * cap.height, 0.0f, 0.0f);

        RC = ray.start;
        RC = XMVectorSubtract(RC, cap.base);
        XMVECTOR dir = XMVectorSubtract(ray.end, ray.start);
        dir = XMVector3Normalize(dir);
        n = XMVector3Cross(dir, axis);

        ln = XMVector3Length(n).m128_f32[0];

        // if parallel
        if ((ln < EPSILON) && (ln > -EPSILON))
            return RayCallback();

        n = XMVector3Normalize(n);
        d = fabsf(XMVector3Dot(RC, n).m128_f32[0]);

        if (d <= cap.radius)
        {
            O = XMVector3Cross(RC, axis);
            t = -XMVector3Dot(O, n).m128_f32[0] / ln;
            O = XMVector3Cross(n, axis);
            O = XMVector3Normalize(O);
            s = fabs(sqrtf(cap.radius * cap.radius - d * d) / XMVector3Dot(dir, O).m128_f32[0]);

            in = t - s;
            out = t + s;

            float lambda;

            if (in < -EPSILON)
            {
                if (out < -EPSILON)
                    return RayCallback();
                else
                    lambda = out;
            }
            else if (out < -EPSILON)
            {
                lambda = in;
            }
            else if (in < out)
            {
                lambda = in;
            }
            else
            {
                lambda = out;
            }

            // Calculate intersection point
            RayCallback raycallback;
            raycallback.point = ray.start;
            raycallback.point.m128_f32[0] += dir.m128_f32[0] * lambda;
            raycallback.point.m128_f32[1] += dir.m128_f32[1] * lambda;
            raycallback.point.m128_f32[2] += dir.m128_f32[2] * lambda;
            XMVECTOR hb = raycallback.point;

            XMVectorSubtract(hb, cap.base);

            //float scale = XMVector3Dot(hb, axis).m128_f32[0];
            //raycallback.normal.m128_f32[0] = hb.m128_f32[0] - axis.m128_f32[0] * scale;
            //raycallback.normal.m128_f32[1] = hb.m128_f32[1] - axis.m128_f32[1] * scale;
            //raycallback.normal.m128_f32[2] = hb.m128_f32[2] - axis.m128_f32[2] * scale;
            raycallback.normal = XMVector3Normalize(-dir);
            raycallback.distance = lambda;
            raycallback.success = true;

            return raycallback;
        }

        return RayCallback();
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

        bool aabb1_inside = DirectX::XMVector3GreaterOrEqual(aabb1.max, aabb2.max) && DirectX::XMVector3LessOrEqual(aabb1.min, aabb2.min);
        bool aabb2_inside = DirectX::XMVector3GreaterOrEqual(aabb2.max, aabb1.max) && DirectX::XMVector3LessOrEqual(aabb2.min, aabb1.min);

        if (aabb1_inside || aabb2_inside)
            return CollideType::INSIDE;

        return CollideType::INTERSECT;
	}

    static CollideType CapsuleToAABB(AABBShape& aabb, CapsuleShape& capsule)
    {
        auto cap_to_aabb = capsule.GetAsAABB();
        return AABBtoAABB(cap_to_aabb, aabb);
        //RayShape box_to_capsule_ray(aabb.center, capsule.base);
        //box_to_capsule_ray.start.m128_f32[1] = 0.0f;
        //box_to_capsule_ray.end.m128_f32[1] = 0.0f;

        //float box_to_capsule_distance = Distance(box_to_capsule_ray.start, box_to_capsule_ray.end) - capsule.radius;

        //XMVECTOR center_to_corner = aabb.max - aabb.center;
        //center_to_corner.m128_f32[1] = 0.0f;
        //float box_radius = XMVectorGetX(XMVector3Length(center_to_corner));

        //if (box_to_capsule_distance <= box_radius)
        //    return true;

        //return false;
    }

    static CollideType CapsuleToCapsule(CapsuleShape& cap1, CapsuleShape& cap2)
    {
        vector<XMVECTOR> cap1_ab = { cap1.GetTipBaseAB()[3], cap1.GetTipBaseAB()[4] };
        vector<XMVECTOR> cap2_ab = { cap2.GetTipBaseAB()[3], cap2.GetTipBaseAB()[4] };

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

        // Chect if 3 vertices are out of aabb but aabb corner intersecs with triangle.
        // it also can detect if aabb is completely inside of triangle 
        for (int i = 0; i < 4; ++i)
        {
            bool success = RayToTriangle(aabb.vertical_ray[i], triangle).success;

            if (success)
                return CollideType::INTERSECT;
        }

        // Check if 3 vertices are out of aabb and vertical rays are not intersect with triangle,
        // but edges of triangles intersect with aabb plane
        auto edge_rays = triangle.GetEdgeRays();
        auto aabb_tries = aabb.GetTriangles();

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 12; ++j)
            {
                bool success = RayToTriangle(edge_rays[i], aabb_tries[j]).success;
                if (success)
                    return CollideType::INTERSECT;
            }
        }

        return CollideType::OUTSIDE;
    }

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

    static CapsuleCallback CapsuleToTriangle(CapsuleShape& cap, TriangleShape& triangle)
    {
        CapsuleCallback result;
        RayShape a_to_base(cap.GetTipBaseAB()[2], cap.GetTipBaseAB()[1]);

        auto raycallback = RayToTriangle(a_to_base, triangle);
        if (raycallback.success)
        {
            result.reaction = CapsuleCallback::FLOOR;
            result.floor_pos = raycallback.point;
        }
        else
        {
            result.reaction = CapsuleCallback::NONE;
            result.floor_pos = cap.base;
        }

        return result;
    }

    static CapsuleCallback CapsuleToTriangleEx(CapsuleShape& cap, TriangleShape& triangle)
    {
        CapsuleCallback result;
        RayShape a_to_base(cap.GetTipBaseAB()[2], cap.GetTipBaseAB()[1]);

        auto raycallback = RayToTriangle(a_to_base, triangle);
        if (raycallback.success) // floor detected
        {
            result.reaction = CapsuleCallback::FLOOR;
            result.floor_pos = raycallback.point;
            return result;
        }

        // check if wall
        auto capsule_info = cap.GetTipBaseAB();

        XMVECTOR a_to_tri_normal = capsule_info[2] + (triangle.normal * cap.radius * -1.0f);
        RayShape a_to_tri(capsule_info[2], a_to_tri_normal);

        XMVECTOR b_to_tri_normal = capsule_info[3] + (triangle.normal * cap.radius * -1.0f);
        RayShape b_to_tri(capsule_info[3], a_to_tri_normal);

        auto a_raycallback = RayToTriangle(a_to_tri, triangle);
        auto b_raycallback = RayToTriangle(b_to_tri, triangle);

        if (a_raycallback.success || b_raycallback.success)
        {
            XMVECTOR a_to_base = cap.GetTipBaseAB()[1] - cap.GetTipBaseAB()[2];
            XMVECTOR a_to_tri_vector = a_to_tri_normal - capsule_info[2];
            float dot = XMVectorGetX(XMVector3Dot(a_to_base, a_to_tri_vector));
            if (dot <= 0.0001f)
            {
                result.reaction = CapsuleCallback::WALL;
                result.floor_pos = cap.base;
                return result;
            }
        }
        else
        {
            result.reaction = CapsuleCallback::NONE;
            result.floor_pos = cap.base;
            return result;
        }
    }
}