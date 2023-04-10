#include "stdafx.h"
#include "Shape.h"

reality::RayShape::RayShape()
{
    start = {0, 0, 0};
    end = {0, 0, 0};
}

reality::RayShape::RayShape(const XMVECTOR& _start, const XMVECTOR& _end)
{
    start = _XMFLOAT3(_start);
    end = _XMFLOAT3(_end);
}

reality::RayShape::RayShape(const XMFLOAT3& _start, const XMFLOAT3& _end)
{
    start = _start;
    end = _end;
}


XMVECTOR reality::RayShape::GetRayVector()
{
    return _XMVECTOR3(start) - _XMVECTOR3(end);
}

reality::TriangleShape::TriangleShape()
{
    vertex0 = _XMFLOAT3(XMVectorZero());
    vertex1 = _XMFLOAT3(XMVectorZero());
    vertex2 = _XMFLOAT3(XMVectorZero());
}

reality::TriangleShape::TriangleShape(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2)
{
    vertex0 = _XMFLOAT3(XMLoadFloat3(&v0));
    vertex1 = _XMFLOAT3(XMLoadFloat3(&v1));
    vertex2 = _XMFLOAT3(XMLoadFloat3(&v2));

    XMVECTOR edge1 = _XMVECTOR3(vertex1) - _XMVECTOR3(vertex0);
    XMVECTOR edge2 = _XMVECTOR3(vertex2) - _XMVECTOR3(vertex0);

    normal = _XMFLOAT3(XMVector3Normalize(XMVector3Cross(edge1, edge2)));
}

bool reality::TriangleShape::SameSide(XMVECTOR p1, XMVECTOR p2, XMVECTOR a, XMVECTOR b)
{
    XMVECTOR cp1 = XMVector3Cross(b - a, p1 - a);
    XMVECTOR cp2 = XMVector3Cross(b - a, p2 - a);
    if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
        return true;
    else
        return false;
}

array<reality::RayShape, 3> reality::TriangleShape::GetEdgeRays()
{
    RayShape edge0(_XMVECTOR3(vertex0), _XMVECTOR3(vertex1));
    RayShape edge1(_XMVECTOR3(vertex1), _XMVECTOR3(vertex2));
    RayShape edge2(_XMVECTOR3(vertex2), _XMVECTOR3(vertex0));

    return { edge0, edge1, edge2 };
}

XMVECTOR reality::TriangleShape::GetMinXZ()
{
    XMVECTOR min_xz = XMVectorZero();

    min_xz.m128_f32[0] = vertex0.x;
    min_xz.m128_f32[0] = min(min_xz.m128_f32[0], vertex1.x);
    min_xz.m128_f32[0] = min(min_xz.m128_f32[0], vertex2.x);

    min_xz.m128_f32[2] = vertex0.z;
    min_xz.m128_f32[2] = min(min_xz.m128_f32[2], vertex1.z);
    min_xz.m128_f32[2] = min(min_xz.m128_f32[2], vertex2.z);

    return min_xz;
}

XMVECTOR reality::TriangleShape::GetMaxXZ()
{
    XMVECTOR max_xz = XMVectorZero();

    max_xz.m128_f32[0] = vertex0.x;
    max_xz.m128_f32[0] = max(max_xz.m128_f32[0], vertex1.x);
    max_xz.m128_f32[0] = max(max_xz.m128_f32[0], vertex2.x);

    max_xz.m128_f32[2] = vertex0.z;
    max_xz.m128_f32[2] = max(max_xz.m128_f32[2],vertex1.z);
    max_xz.m128_f32[2] = max(max_xz.m128_f32[2],vertex2.z);

    return max_xz;
}

reality::AABBShape::AABBShape(const XMFLOAT3& _min, const XMFLOAT3& _max)
{
    min = _min;
    max = _max;
    center = _XMFLOAT3((_XMVECTOR3(_min) + _XMVECTOR3(_max) / 2));
    corner = GetCorners();
    triangle = GetTriangles();
    vertical_ray = GetYAxisRay();
}

reality::AABBShape::AABBShape(const XMFLOAT3& _center, const float& scale)
{
    center = _center;
    min = _XMFLOAT3((_XMVECTOR3(center) - XMVectorSet(scale / 2, scale / 2, scale / 2, 0)));
    max = _XMFLOAT3((_XMVECTOR3(center) + XMVectorSet(scale / 2, scale / 2, scale / 2, 0)));
    corner = GetCorners();
    triangle = GetTriangles();
    vertical_ray = GetYAxisRay();
}

array<XMFLOAT3, 8> reality::AABBShape::GetCorners()
{
    array<XMFLOAT3, 8> corners;
    corners[0] = XMFLOAT3(min.x, min.y, min.z);
    corners[1] = XMFLOAT3(min.x, min.y, max.z);
    corners[2] = XMFLOAT3(min.x, max.y, min.z);
    corners[3] = XMFLOAT3(min.x, max.y, max.z);
    corners[4] = XMFLOAT3(max.x, min.y, min.z);
    corners[5] = XMFLOAT3(max.x, min.y, max.z);
    corners[6] = XMFLOAT3(max.x, max.y, min.z);
    corners[7] = XMFLOAT3(max.x, max.y, max.z);

    return corners;
}

array<reality::TriangleShape, 12> reality::AABBShape::GetTriangles()
{
    XMFLOAT3* corners = GetCorners().data();
    array<TriangleShape, 12> triangles;

    triangles[0]  = TriangleShape(corners[0], corners[1], corners[2]);
    triangles[1]  = TriangleShape(corners[2], corners[3], corners[0]);
    triangles[2]  = TriangleShape(corners[7], corners[6], corners[5]);
    triangles[3]  = TriangleShape(corners[5], corners[4], corners[7]);
    triangles[4]  = TriangleShape(corners[1], corners[5], corners[6]);
    triangles[5]  = TriangleShape(corners[6], corners[2], corners[1]);
    triangles[6]  = TriangleShape(corners[4], corners[0], corners[3]);
    triangles[7]  = TriangleShape(corners[3], corners[7], corners[4]);
    triangles[8]  = TriangleShape(corners[4], corners[5], corners[1]);
    triangles[9]  = TriangleShape(corners[1], corners[0], corners[4]);
    triangles[10] = TriangleShape(corners[3], corners[2], corners[6]);
    triangles[11] = TriangleShape(corners[6], corners[7], corners[3]);

    return triangles;
}

array<reality::RayShape, 4> reality::AABBShape::GetYAxisRay()
{
    XMFLOAT3* corners = GetCorners().data();
    array<RayShape, 4> y_ray;
    y_ray[0] = RayShape(_XMVECTOR3(corners[2]), _XMVECTOR3(corners[0]));
    y_ray[1] = RayShape(_XMVECTOR3(corners[6]), _XMVECTOR3(corners[4]));
    y_ray[2] = RayShape(_XMVECTOR3(corners[3]), _XMVECTOR3(corners[1]));
    y_ray[3] = RayShape(_XMVECTOR3(corners[7]), _XMVECTOR3(corners[5]));

    return y_ray;
}

reality::SphereShape::SphereShape(const XMFLOAT3& _center, const float& _radius)
{
    center = _center;
    radius = _radius;
}

reality::SphereShape::SphereShape(const AABBShape& _aabb)
{
    center = _XMFLOAT3(((_XMVECTOR3(_aabb.min) + _XMVECTOR3(_aabb.max)) / 2));
    radius = Vector3Length((_XMVECTOR3(_aabb.max) - _XMVECTOR3(center)));
}

reality::CapsuleShape::CapsuleShape()
{
    base = _XMFLOAT3(XMVectorZero());
    height = 0.0f;
    radius = 0.0f;
}

reality::CapsuleShape::CapsuleShape(const XMFLOAT3& _base, const float& _height, const float& _radius)
{
    base = _base;
    height = _height;
    radius = _radius;
}

array<XMVECTOR, 4> reality::CapsuleShape::GetTipBaseAB()
{
    XMVECTOR tip = _XMVECTOR3(base) + XMVectorSet(0, height, 0, 0);

    XMVECTOR normal = XMVector3Normalize(tip - _XMVECTOR3(base));
    XMVECTOR lineend = normal * radius;
    XMVECTOR A = _XMVECTOR3(base) + lineend;
    XMVECTOR B = tip - lineend;

    return { tip, _XMVECTOR3(base), A, B };
}

reality::AABBShape reality::CapsuleShape::GetAsAABB()
{
    XMVECTOR cap_min = _XMVECTOR3(base) + XMVectorSet(-radius, 0, -radius, 0);
    XMVECTOR cap_max = _XMVECTOR3(base) + XMVectorSet(radius, height, radius, 0);
    return AABBShape(_XMFLOAT3(cap_min), _XMFLOAT3(cap_max));
}

reality::PlaneShape::PlaneShape(XMVECTOR vec0, XMVECTOR vec1, XMVECTOR vec2)
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

float reality::PlaneShape::DotFromPoint(XMVECTOR point)
{
    float distance =
        a * point.m128_f32[0] +
        b * point.m128_f32[1] +
        c * point.m128_f32[2] +
        d;

    return distance;
}

reality::Frustum::Frustum(const XMMATRIX& mat_view_proj)
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

    topbottom_tries[0] = TriangleShape(_XMFLOAT3(frustum_vertex[5]), _XMFLOAT3(frustum_vertex[6]), _XMFLOAT3(frustum_vertex[1]));
    topbottom_tries[1] = TriangleShape(_XMFLOAT3(frustum_vertex[1]), _XMFLOAT3(frustum_vertex[6]), _XMFLOAT3(frustum_vertex[2]));
    topbottom_tries[2] = TriangleShape(_XMFLOAT3(frustum_vertex[0]), _XMFLOAT3(frustum_vertex[3]), _XMFLOAT3(frustum_vertex[4]));
    topbottom_tries[3] = TriangleShape(_XMFLOAT3(frustum_vertex[4]), _XMFLOAT3(frustum_vertex[3]), _XMFLOAT3(frustum_vertex[7]));
}