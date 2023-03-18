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
        array<RayShape, 3> GetEdgeRays()
        {
            RayShape edge0(vertex0, vertex1);
            RayShape edge1(vertex1, vertex2);
            RayShape edge2(vertex2, vertex0);

            return { edge0, edge1, edge2 };
        }
        XMVECTOR vertex0, vertex1, vertex2;
        XMVECTOR normal;
        UINT index;
    };

    struct AABBShape
    {    
        AABBShape() {}
        AABBShape(const XMVECTOR& _min, const XMVECTOR& _max)
        {
            min = _min;
            max = _max;
            center = (min + max) / 2;
            corner = GetCorners();
            triangle = GetTriangles();
            vertical_ray = GetYAxisRay();
        }
        AABBShape(const XMVECTOR& _center, const float& scale)
        {
            center = _center;
            min = center - XMVectorSet(scale / 2, scale / 2, scale / 2, 0);
            max = center + XMVectorSet(scale / 2, scale / 2, scale / 2, 0);
            corner = GetCorners();
            triangle = GetTriangles();
            vertical_ray = GetYAxisRay();
        }
        array<XMVECTOR, 8> GetCorners()
        {
            array<XMVECTOR, 8> corners;
            corners[0] = XMVectorSet(XMVectorGetX(min), XMVectorGetY(min), XMVectorGetZ(min), 1);
            corners[1] = XMVectorSet(XMVectorGetX(min), XMVectorGetY(min), XMVectorGetZ(max), 1);
            corners[2] = XMVectorSet(XMVectorGetX(min), XMVectorGetY(max), XMVectorGetZ(min), 1);
            corners[3] = XMVectorSet(XMVectorGetX(min), XMVectorGetY(max), XMVectorGetZ(max), 1);
            corners[4] = XMVectorSet(XMVectorGetX(max), XMVectorGetY(min), XMVectorGetZ(min), 1);
            corners[5] = XMVectorSet(XMVectorGetX(max), XMVectorGetY(min), XMVectorGetZ(max), 1);
            corners[6] = XMVectorSet(XMVectorGetX(max), XMVectorGetY(max), XMVectorGetZ(min), 1);
            corners[7] = XMVectorSet(XMVectorGetX(max), XMVectorGetY(max), XMVectorGetZ(max), 1);
        
            return corners;
        }
        array<TriangleShape, 12> GetTriangles()
        {
            XMVECTOR* corners = GetCorners().data();
            array<TriangleShape, 12> triangles;

            triangles[0] = TriangleShape(corners[0], corners[1], corners[2]);
            triangles[1] = TriangleShape(corners[2], corners[3], corners[0]);
            triangles[2] = TriangleShape(corners[7], corners[6], corners[5]);
            triangles[3] = TriangleShape(corners[5], corners[4], corners[7]);
            triangles[4] = TriangleShape(corners[1], corners[5], corners[6]);
            triangles[5] = TriangleShape(corners[6], corners[2], corners[1]);
            triangles[6] = TriangleShape(corners[4], corners[0], corners[3]);
            triangles[7] = TriangleShape(corners[3], corners[7], corners[4]);
            triangles[8] = TriangleShape(corners[4], corners[5], corners[1]);
            triangles[9] = TriangleShape(corners[1], corners[0], corners[4]);
            triangles[10] = TriangleShape(corners[3], corners[2], corners[6]);
            triangles[11] = TriangleShape(corners[6], corners[7], corners[3]);

            return triangles;
        }
        array<RayShape, 4> GetYAxisRay()
        {
            XMVECTOR* corners = GetCorners().data();
            array<RayShape, 4> y_ray;
            y_ray[0] = RayShape(corners[2], corners[0]);
            y_ray[1] = RayShape(corners[6], corners[4]);
            y_ray[2] = RayShape(corners[3], corners[1]);
            y_ray[3] = RayShape(corners[7], corners[5]);

            return y_ray;
        }

        XMVECTOR min, max, center;  
        array<XMVECTOR, 8>       corner;
        array<TriangleShape, 12> triangle;
        array<RayShape, 4>       vertical_ray;
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
            height = 0.0f;
            radius = 0.0f;
        }
        CapsuleShape(const XMVECTOR& _base, const float& _height, const float& _radius)
        {
            base = _base;
            height = _height;
            radius = _radius;
        }
        array<XMVECTOR, 4> GetTipBaseAB()
        {
            XMVECTOR tip = base + XMVectorSet(0, height, 0, 0);

            XMVECTOR normal = XMVector3Normalize(tip - base);
            XMVECTOR lineend = normal * radius;
            XMVECTOR A = base + lineend;
            XMVECTOR B = tip - lineend;

            return { tip, base, A, B };
        }
        XMVECTOR base;
        float height;
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
            frustum_plane[4] = PlaneShape(frustum_vertex[1], frustum_vertex[3], frustum_vertex[2]);
            frustum_plane[5] = PlaneShape(frustum_vertex[6], frustum_vertex[4], frustum_vertex[5]);

            topbottom_tries[0] = TriangleShape(frustum_vertex[5], frustum_vertex[6], frustum_vertex[1]);
            topbottom_tries[1] = TriangleShape(frustum_vertex[1], frustum_vertex[6], frustum_vertex[2]);
            topbottom_tries[2] = TriangleShape(frustum_vertex[0], frustum_vertex[3], frustum_vertex[4]);
            topbottom_tries[3] = TriangleShape(frustum_vertex[4], frustum_vertex[3], frustum_vertex[7]);
        }

        XMVECTOR frustum_vertex[8];
        PlaneShape  frustum_plane[6];
        TriangleShape topbottom_tries[4];
    };

}
