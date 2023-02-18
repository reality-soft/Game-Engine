#pragma once
#include"stdafx.h"

namespace KGCA41B {

    enum class OverlapType
    {
        OUTSIDE,
        INSIDE,
        PARTLY,
    };

    struct AABBShape
    {    
        AABBShape() {}
        AABBShape(const XMVECTOR& _min, const XMVECTOR& _max)
        {
            min = _min;
            max = _max;
        }

        XMVECTOR min, max;
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
            XMVECTOR e1 = vec1 - vec0;
            XMVECTOR e2 = vec2 - vec0;

            normal = XMVector3Normalize(XMVector3Cross(e1, e2));

            a = normal.m128_f32[0];
            b = normal.m128_f32[0];
            c = normal.m128_f32[0];
            d = -XMVector3Dot(normal, vec0).m128_f32[0];
        }
        bool PointOverlap(XMVECTOR point)
        {
            for (int i = 0; i < 6; ++i)
            {
                float distance = 
                    a * point.m128_f32[0] +
                    b * point.m128_f32[1] +
                    c * point.m128_f32[2] +
                    d;

                if (distance < 0)
                {
                    return false;
                }
            }
            return true;
        }
        FLOAT a, b, c, d;
        XMVECTOR normal;
    };

    struct Frustum
    {
        Frustum() {}
        Frustum(const XMMATRIX& mat_view, const XMMATRIX& mat_porj)
        {
            XMMATRIX mat_vp = XMMatrixInverse(nullptr, mat_view * mat_porj);

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
            OverlapType result = OverlapType::PARTLY;
            int total_in = 0;

            XMVECTOR corners[8];
            corners[0] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.min), XMVectorGetZ(other.min), 1);
            corners[1] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.min), XMVectorGetZ(other.max), 1);
            corners[2] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.max), XMVectorGetZ(other.min), 1);
            corners[3] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.max), XMVectorGetZ(other.max), 1);
            corners[4] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.min), XMVectorGetZ(other.min), 1);
            corners[5] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.min), XMVectorGetZ(other.max), 1);
            corners[6] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.max), XMVectorGetZ(other.min), 1);
            corners[7] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.max), XMVectorGetZ(other.max), 1);


            for (int p = 0; p < 6; ++p)
            {
                int in_corner = 8;
                int in_plane = 1;

                for (int b = 0; b < 8; ++b) {

                    // test this point against the planes
                    if (frustum_plane[p].PointOverlap(corners[b]) == false) {
                        in_plane = 0;
                        --in_corner;
                    }
                }
                if(in_corner == 0)
                    return OverlapType::OUTSIDE;

                total_in += in_plane;
            }
            if (total_in == 6)
                return OverlapType::INSIDE;

            return OverlapType::PARTLY;
        }

        XMVECTOR frustum_vertex[8];
        PlaneShape  frustum_plane[6];
    };

}
