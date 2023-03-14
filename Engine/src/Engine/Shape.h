#pragma once
#include"stdafx.h"
#define EPSILON 0.00001f

namespace reality {

    struct RayShape
    {
        RayShape()
        {
            start = XMVectorZero();
            end = XMVectorZero();
        }
        RayShape(const XMVECTOR& _start, const XMVECTOR& _end)
        {
            start = _start;
            end = _end;
        }
        XMVECTOR start, end;
    };

    struct TriangleShape
    {
        TriangleShape() 
        {
            vertex0 = XMVectorZero();
            vertex1 = XMVectorZero();
            vertex2 = XMVectorZero();
        }
        TriangleShape(const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2)
        {
            vertex0 = v0;
            vertex1 = v1;
            vertex2 = v2;

            XMVECTOR edge1 = vertex1 - vertex0;
            XMVECTOR edge2 = vertex2 - vertex0;

            normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));
        }
        TriangleShape(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2)
        {
            vertex0 = XMLoadFloat3(&v0);
            vertex1 = XMLoadFloat3(&v1);
            vertex2 = XMLoadFloat3(&v2);
            
            XMVECTOR edge1 = vertex1 - vertex0;
            XMVECTOR edge2 = vertex2 - vertex0;

            normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));
        }
        bool SameSide(XMVECTOR p1, XMVECTOR p2, XMVECTOR a, XMVECTOR b)
        {
            XMVECTOR cp1 = XMVector3Cross(b - a, p1 - a);
            XMVECTOR cp2 = XMVector3Cross(b - a, p2 - a);
            if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
                return true;
            else
                return false;
        }
        XMVECTOR vertex0, vertex1, vertex2;
        XMVECTOR normal;
    };

    struct AABBShape
    {    
        AABBShape() {}
        AABBShape(const XMVECTOR& _min, const XMVECTOR& _max)
        {
            min = _min;
            max = _max;
            center = (min + max) / 2;
            ConvertTries();
        }
        AABBShape(const XMVECTOR& _center, const float& scale)
        {
            center = _center;
            min = center - XMVectorSet(scale / 2, scale / 2, scale / 2, 0);
            max = center + XMVectorSet(scale / 2, scale / 2, scale / 2, 0);
            ConvertTries();
        }
        void ConvertTries()
        {
            XMVECTOR corners[8];
            corners[0] = XMVectorSet(XMVectorGetX(min), XMVectorGetY(min), XMVectorGetZ(min), 1);
            corners[1] = XMVectorSet(XMVectorGetX(min), XMVectorGetY(min), XMVectorGetZ(max), 1);
            corners[2] = XMVectorSet(XMVectorGetX(min), XMVectorGetY(max), XMVectorGetZ(min), 1);
            corners[3] = XMVectorSet(XMVectorGetX(min), XMVectorGetY(max), XMVectorGetZ(max), 1);
            corners[4] = XMVectorSet(XMVectorGetX(max), XMVectorGetY(min), XMVectorGetZ(min), 1);
            corners[5] = XMVectorSet(XMVectorGetX(max), XMVectorGetY(min), XMVectorGetZ(max), 1);
            corners[6] = XMVectorSet(XMVectorGetX(max), XMVectorGetY(max), XMVectorGetZ(min), 1);
            corners[7] = XMVectorSet(XMVectorGetX(max), XMVectorGetY(max), XMVectorGetZ(max), 1);

            triangle[0]  = TriangleShape(corners[0], corners[1], corners[2]);
            triangle[1]  = TriangleShape(corners[2], corners[3], corners[0]);

            triangle[2]  = TriangleShape(corners[7], corners[6], corners[5]);
            triangle[3]  = TriangleShape(corners[5], corners[4], corners[7]);

            triangle[4]  = TriangleShape(corners[1], corners[5], corners[6]);
            triangle[5]  = TriangleShape(corners[6], corners[2], corners[1]);

            triangle[6]  = TriangleShape(corners[4], corners[0], corners[3]);
            triangle[7]  = TriangleShape(corners[3], corners[7], corners[4]);

            triangle[8]  = TriangleShape(corners[4], corners[5], corners[1]);
            triangle[9]  = TriangleShape(corners[1], corners[0], corners[4]);

            triangle[10] = TriangleShape(corners[3], corners[2], corners[6]);
            triangle[11] = TriangleShape(corners[6], corners[7], corners[3]);
        }
        XMVECTOR min, max, center;
        TriangleShape triangle[12];
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

    struct CapsuleShape
    {
        CapsuleShape()
        {
            base = XMVectorZero();
            tip = XMVectorZero();
            radius = 0.0f;
        }
        CapsuleShape(float _min, float _max, float _radius)
        {
            base = XMVectorSet(0, _min, 0, 0);
            tip = XMVectorSet(0, _max, 0, 0);
            radius = _radius;
        }
        CapsuleShape(const XMVECTOR& _base, const XMVECTOR& _tip, const float& _radius)
        {
            base = _base;
            tip = _tip;
            radius = _radius;
        }
        CapsuleShape(const AABBShape& _aabb)
        {
            base = _aabb.center + XMVectorSet(0, XMVectorGetY(_aabb.min), 0, 0);
            tip = _aabb.center + XMVectorSet(0, XMVectorGetY(_aabb.max), 0, 0);
            XMVECTOR extend = _aabb.max - _aabb.min;
            extend.m128_f32[1] = 0;
            radius = XMVectorGetX(XMVector3Length(extend));
        }
        vector<XMVECTOR> GetAB()
        {
            XMVECTOR normal = XMVector3Normalize(tip - base);
            XMVECTOR lineend = normal * radius;
            XMVECTOR A = base + lineend;
            XMVECTOR B = tip - lineend;

            return { A, B };
        }

        XMVECTOR base, tip;
        float radius;
    };

    struct PlaneShape
    {
        PlaneShape() {}
        PlaneShape(XMVECTOR vec0, XMVECTOR vec1, XMVECTOR vec2)
        {
            XMVECTOR e1 = vec1 - vec0;
            XMVECTOR e2 = vec2 - vec0;
            normal = XMVector3Normalize(XMVector3Cross(e1, e2));

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

        //OverlapType AABBOverlap(const AABBShape& other) // map culling
        //{
        //    int in_corner = 0;
        //    int out_corner = 0;

        //    XMVECTOR corners[12];
        //    corners[0] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.min), XMVectorGetZ(other.min), 1);
        //    corners[1] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.min), XMVectorGetZ(other.max), 1);
        //    corners[2] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.max), XMVectorGetZ(other.min), 1);
        //    corners[3] = XMVectorSet(XMVectorGetX(other.min), XMVectorGetY(other.max), XMVectorGetZ(other.max), 1);
        //    corners[4] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.min), XMVectorGetZ(other.min), 1);
        //    corners[5] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.min), XMVectorGetZ(other.max), 1);
        //    corners[6] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.max), XMVectorGetZ(other.min), 1);
        //    corners[7] = XMVectorSet(XMVectorGetX(other.max), XMVectorGetY(other.max), XMVectorGetZ(other.max), 1);

        //    float plane_from_center = 0;

        //    for (int i = 0; i < 6; ++i)
        //    {
        //        plane_from_center += frustum_plane[i].DotFromPoint(other.center);

        //        float dot = 0;
        //        for (int j = 0; j < 8; ++j)
        //        {
        //            dot = frustum_plane[i].DotFromPoint(corners[j]);

        //            if (dot < 0) out_corner++;
        //            else in_corner++;
        //        }

        //        if (in_corner == 0)
        //        {
        //            return OverlapType::OUTSIDE;
        //        }
        //        if (out_corner == 0)
        //        {
        //            return OverlapType::INSIDE;
        //        }

        //    }
        //    plane_from_center;
        //    return OverlapType::INTERSECT;
        //}

        XMVECTOR frustum_vertex[8];
        PlaneShape  frustum_plane[6];
    };

}
