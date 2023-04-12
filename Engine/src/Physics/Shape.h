#pragma once
#define EPSILON 0.00001f

namespace reality {

    struct DLL_API RayShape
    {
        RayShape();
        RayShape(const XMVECTOR& _start, const XMVECTOR& _end);
        RayShape(const XMFLOAT3& _start, const XMFLOAT3& _end);
        XMVECTOR GetRayVector();
        XMFLOAT3 start, end;
    };

    struct DLL_API TriangleShape
    {
        TriangleShape();
        TriangleShape(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2);

        bool SameSide(XMVECTOR p1, XMVECTOR p2, XMVECTOR a, XMVECTOR b);
        array<RayShape, 3> GetEdgeRays();
        XMVECTOR GetMinXZ();
        XMVECTOR GetMaxXZ();

        UINT index;
        XMFLOAT3 normal;
        XMFLOAT3 vertex0, vertex1, vertex2;
    };

    struct DLL_API AABBShape
    {    
        AABBShape() {}
        AABBShape(const XMFLOAT3& _min, const XMFLOAT3& _max);
        AABBShape(const XMFLOAT3& _center, const float& scale);
        array<XMFLOAT3, 8> GetCorners();
        array<TriangleShape, 12> GetTriangles();
        array<RayShape, 4> GetYAxisRay();

        XMFLOAT3 min, max, center;
    };

    struct DLL_API SphereShape
    {
        SphereShape() {}
        SphereShape(const XMFLOAT3& _center, const float& _radius);
        SphereShape(const AABBShape& _aabb);

        float radius;
        XMFLOAT3 center;
    };

    struct DLL_API CapsuleShape
    {
        CapsuleShape();
        CapsuleShape(const XMFLOAT3& _base, const float& _height, const float& _radius);
        array<XMVECTOR, 4> GetTipBaseAB();
        AABBShape GetAsAABB();

        float height;
        float radius;
        XMFLOAT3 base;
    };

    struct DLL_API PlaneShape
    {
        PlaneShape() {}
        PlaneShape(XMVECTOR vec0, XMVECTOR vec1, XMVECTOR vec2);
        float DotFromPoint(XMVECTOR point);

        FLOAT a, b, c, d;
        XMVECTOR normal;
    };

    struct DLL_API Frustum
    {
        Frustum() {}
        Frustum(const XMMATRIX& mat_view_proj);

        XMVECTOR frustum_vertex[8];
        PlaneShape  frustum_plane[6];
        TriangleShape topbottom_tries[4];
    };

}