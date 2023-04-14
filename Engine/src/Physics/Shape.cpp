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

reality::AABBShape::AABBShape(const XMFLOAT3& _min, const XMFLOAT3& _max)
{
    min = _min;
    max = _max;
    center = _XMFLOAT3((_XMVECTOR3(_min) + _XMVECTOR3(_max) / 2));
}

reality::AABBShape::AABBShape(const XMFLOAT3& _center, const float& scale)
{
    center = _center;
    min = _XMFLOAT3((_XMVECTOR3(center) - XMVectorSet(scale / 2, scale / 2, scale / 2, 0)));
    max = _XMFLOAT3((_XMVECTOR3(center) + XMVectorSet(scale / 2, scale / 2, scale / 2, 0)));
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

void reality::ConvertToTrianlgeShapes(vector<TriangleShape>& out, const SingleMesh<Vertex>& mesh)
{
    UINT num_triangle = mesh.vertices.size() / 3;
    UINT index = 0;
    for (UINT t = 0; t < num_triangle; t++)
    {
        TriangleShape tri_plane = TriangleShape(
            mesh.vertices[index + 0].p,
            mesh.vertices[index + 2].p,
            mesh.vertices[index + 1].p
        );

        tri_plane.index = t;

        out.push_back(tri_plane);
        index += 3;
    }
    
}

void reality::ConvertToAABBShape(AABBShape& out, const SingleMesh<Vertex>& mesh)
{
    XMFLOAT3 aabb_min = {0, 0, 0};
    XMFLOAT3 aabb_max = {0, 0, 0};

    for (const auto vertex : mesh.vertices)
    {
        aabb_min.x = min(aabb_min.x, vertex.p.x);
        aabb_min.y = min(aabb_min.y, vertex.p.y);
        aabb_min.y = min(aabb_min.z, vertex.p.z);

        aabb_max.x = max(aabb_max.x, vertex.p.x);
        aabb_max.y = max(aabb_max.y, vertex.p.y);
        aabb_max.y = max(aabb_max.z, vertex.p.z);
    }

    out = AABBShape(aabb_min, aabb_max);
}
