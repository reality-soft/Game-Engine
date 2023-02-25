#pragma once
#include"stdafx.h"

namespace KGCA41B {

    enum class OverlapType
    {
        OUTSIDE,
        INTERSECT,
        INSIDE,
    };

    struct AABBShape
    {    
        AABBShape() {}
        AABBShape(const XMVECTOR& _min, const XMVECTOR& _max)
        {
            min = _min;
            max = _max;
            center = (min + max) / 2;
        }
        AABBShape(const XMVECTOR& _center, const float& scale)
        {
            center = _center;
            min = center - XMVectorSet(scale / 2, scale / 2, scale / 2, 0);
            max = center + XMVectorSet(scale / 2, scale / 2, scale / 2, 0);
        }
        OverlapType AABBOverlap(const AABBShape& other)
        {
            for (int i = 0; i < 3; i++) 
            {
                bool overlap = max.m128_f32[i] >= other.min.m128_f32[i] && min.m128_f32[i] <= other.max.m128_f32[i];
                if (!overlap)
                    return OverlapType::OUTSIDE;
            }

            bool aabb1_inside = DirectX::XMVector3GreaterOrEqual(max, other.max) && DirectX::XMVector3LessOrEqual(min, other.max);
            bool aabb2_inside = DirectX::XMVector3GreaterOrEqual(other.max, max) && DirectX::XMVector3LessOrEqual(other.max, min);

            if (aabb1_inside || aabb2_inside)
                return OverlapType::INSIDE;

            return OverlapType::INTERSECT;            
        }
        XMVECTOR min, max, center;
    };

    struct SphereShape
    {
        SphereShape() {}
        SphereShape(const XMVECTOR& _center, const FLOAT& _radius)
        {
            center = _center;
            radius = _radius;
        }
        SphereShape(const AABBShape& _aabb)
        {
            center = (_aabb.min + _aabb.max) / 2;
            radius = XMVector3Length(_aabb.max - center).m128_f32[0];
        }

        XMVECTOR center;
        FLOAT radius;
    };

    struct PlaneShape
    {
        PlaneShape() {}
        PlaneShape(XMVECTOR vec0, XMVECTOR vec1, XMVECTOR vec2)
        {
            XMVECTOR plane = XMPlaneFromPoints(vec0, vec1, vec2);

            a = plane.m128_f32[0];
            b = plane.m128_f32[1];
            c = plane.m128_f32[2];
            d = plane.m128_f32[3];
        }
        float DotFromPoint(XMVECTOR point)
        {
            float distance = 
                a * point.m128_f32[0] +
                b * point.m128_f32[1] +
                c * point.m128_f32[2] +
                d;

            return distance;
        }
        FLOAT a, b, c, d;
        XMVECTOR normal;
    };

    struct Frustum
    {
        Frustum() {}
        Frustum(const XMMATRIX& mat_view_proj)
        {
            XMMATRIX mat_vp = XMMatrixInverse(nullptr, mat_view_proj);

            // near plane
            frustum_vertex[0] = XMVectorSet(-1.0f, -1.0f, 0.0f, 0);
            frustum_vertex[1] = XMVectorSet(-1.0f, 1.0f, 0.0f, 0);
            frustum_vertex[2] = XMVectorSet(1.0f, 1.0f, 0.0f, 0);
            frustum_vertex[3] = XMVectorSet(1.0f, -1.0f, 0.0f, 0);

            // far plane
            frustum_vertex[4] = XMVectorSet(-1.0f, -1.0f, 1.0f, 0);
            frustum_vertex[5] = XMVectorSet(-1.0f, 1.0f, 1.0f, 0);
            frustum_vertex[6] = XMVectorSet(1.0f, 1.0f, 1.0f, 0);
            frustum_vertex[7] = XMVectorSet(1.0f, -1.0f, 1.0f, 0);

            for (int i = 0; i < 8; ++i)
            {
                frustum_vertex[i] = XMVector3TransformCoord(frustum_vertex[i], mat_vp);
            }

            frustum_plane[0] = PlaneShape(frustum_vertex[1], frustum_vertex[5], frustum_vertex[0]);
            frustum_plane[1] = PlaneShape(frustum_vertex[3], frustum_vertex[6], frustum_vertex[2]);
            frustum_plane[2] = PlaneShape(frustum_vertex[5], frustum_vertex[2], frustum_vertex[6]);
            frustum_plane[3] = PlaneShape(frustum_vertex[0], frustum_vertex[7], frustum_vertex[3]);
            frustum_plane[4] = PlaneShape(frustum_vertex[2], frustum_vertex[1], frustum_vertex[0]);
            frustum_plane[5] = PlaneShape(frustum_vertex[5], frustum_vertex[6], frustum_vertex[4]);
        }

        OverlapType AABBOverlap(const AABBShape& other) // map culling
        {
            int in_corner = 0;
            int out_corner = 0;

            XMVECTOR corners[12];
            corners[0] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.min), XMVectorGetZ(other.min), 1);
            corners[1] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.min), XMVectorGetZ(other.max), 1);
            corners[2] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.max), XMVectorGetZ(other.min), 1);
            corners[3] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.max), XMVectorGetZ(other.max), 1);
            corners[4] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.min), XMVectorGetZ(other.min), 1);
            corners[5] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.min), XMVectorGetZ(other.max), 1);
            corners[6] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.max), XMVectorGetZ(other.min), 1);
            corners[7] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.max), XMVectorGetZ(other.max), 1);

            float plane_from_center = 0;

            for (int i = 0; i < 6; ++i)
            {
                plane_from_center += frustum_plane[i].DotFromPoint(other.center);

                float dot = 0;
                for (int j = 0; j < 8; ++j)
                {
                    dot = frustum_plane[i].DotFromPoint(corners[j]);

                    if (dot < 0) out_corner++;
                    else in_corner++;
                }

                if (in_corner == 0)
                {
                    return OverlapType::OUTSIDE;
                }
                if (out_corner == 0)
                {
                    return OverlapType::INSIDE;
                }

            }
            plane_from_center;
            return OverlapType::INTERSECT;
        }

        XMVECTOR frustum_vertex[8];
        PlaneShape  frustum_plane[6];
    };

}
