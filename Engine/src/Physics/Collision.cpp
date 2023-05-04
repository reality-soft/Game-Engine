#include "stdafx.h"
#include "Collision.h"

using namespace reality;

bool reality::SameSide(XMVECTOR p1, XMVECTOR p2, XMVECTOR a, XMVECTOR b)
{
    XMVECTOR cp1 = XMVector3Cross(b - a, p1 - a);
    XMVECTOR cp2 = XMVector3Cross(b - a, p2 - a);
    if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
        return true;
    else
        return false;
}

bool reality::PointInTriangle(const XMVECTOR& p, const TriangleShape& tri)
{
    XMVECTOR tri_vertex0 = _XMVECTOR3(tri.vertex0);
    XMVECTOR tri_vertex1 = _XMVECTOR3(tri.vertex1);
    XMVECTOR tri_vertex2 = _XMVECTOR3(tri.vertex2);
    XMVECTOR tri_normal = _XMVECTOR3(tri.normal);

    if (SameSide(p, tri_vertex0, tri_vertex1, tri_vertex2) &&
        SameSide(p, tri_vertex1, tri_vertex0, tri_vertex2) &&
        SameSide(p, tri_vertex2, tri_vertex0, tri_vertex1))
    {
        XMVECTOR cross = XMVector3Cross(tri_vertex0 - p, tri_vertex1 - p);
        if (IsParallelVector(cross, tri_normal))
            return true;
    }
    return false;
}

RayCallback reality::RayToTriangle(const RayShape& ray, const TriangleShape& tri)
{
    XMVECTOR tri_vertex0 = _XMVECTOR3(tri.vertex0);
    XMVECTOR tri_vertex1 = _XMVECTOR3(tri.vertex1);
    XMVECTOR tri_vertex2 = _XMVECTOR3(tri.vertex2);
    XMVECTOR tri_normal = _XMVECTOR3(tri.normal);

    RayCallback callback;
    XMVECTOR P = XMPlaneIntersectLine(XMPlaneFromPoints(tri_vertex0, tri_vertex1, tri_vertex2), _XMVECTOR3(ray.start), _XMVECTOR3(ray.end));

    if (PointInTriangle(P, tri))
    {
        float distance = Distance(P, _XMVECTOR3(ray.start));
        float dot_product = XMVector3Dot(GetRayDirection(ray), P - _XMVECTOR3(ray.start)).m128_f32[0];
        float ray_length = Distance(_XMVECTOR3(ray.end), _XMVECTOR3(ray.start));

        if (distance <= ray_length && dot_product > 0)
        {
            callback.success = true;
            callback.distance = distance;
            callback.point = P;
            callback.normal = tri_normal;
            callback.tri_index = tri.index;
        }
    }

    return callback;
}

bool reality::RayToAABB(const RayShape& ray, const AABBShape& aabb)
{
    XMVECTOR center_to_corner = _XMVECTOR3(aabb.max) - _XMVECTOR3(aabb.center);
    center_to_corner.m128_f32[1] = 0.0f;
    float box_radius = XMVectorGetX(XMVector3Length(center_to_corner));

    XMVECTOR line1 = _XMVECTOR3(ray.start); line1.m128_f32[1] = aabb.center.y;
    XMVECTOR line2 = _XMVECTOR3(ray.end); line2.m128_f32[1] = aabb.center.y;
    float box_to_line = XMVectorGetX(XMVector3LinePointDistance(line1, line2, _XMVECTOR3(aabb.center)));

    if (box_to_line <= box_radius)
        return true;

    return false;
}

RayCallback reality::RayToCapsule(const RayShape& ray, const CapsuleShape& cap)
{
    XMVECTOR RC;
    float d;
    float t, s;
    XMVECTOR n, D, O;
    float ln;
    float in, out;

    XMVECTOR axis = XMVectorSet(0.0f, 1.0f * cap.height, 0.0f, 0.0f);

    RC = _XMVECTOR3(ray.start);
    RC = XMVectorSubtract(RC, _XMVECTOR3(cap.base));
    XMVECTOR dir = GetRayDirection(ray);
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
        raycallback.point = _XMVECTOR3(ray.start);
        raycallback.point.m128_f32[0] += dir.m128_f32[0] * lambda;
        raycallback.point.m128_f32[1] += dir.m128_f32[1] * lambda;
        raycallback.point.m128_f32[2] += dir.m128_f32[2] * lambda;
        XMVECTOR hb = raycallback.point;

        XMVectorSubtract(hb, _XMVECTOR3(cap.base));

        raycallback.normal = XMVector3Normalize(-dir);
        raycallback.distance = lambda;
        raycallback.success = true;

        return raycallback;
    }

    return RayCallback();
}

CollideType reality::AABBtoAABB(const AABBShape& aabb1, const AABBShape& aabb2)
{
    for (int i = 0; i < 3; i++)
    {
        bool overlap =
            _XMVECTOR3(aabb1.max).m128_f32[i] >= _XMVECTOR3(aabb2.min).m128_f32[i]
            &&
            _XMVECTOR3(aabb1.min).m128_f32[i] <= _XMVECTOR3(aabb2.max).m128_f32[i];

        if (!overlap)
            return CollideType::OUTSIDE;
    }

    bool aabb1_inside = DirectX::XMVector3GreaterOrEqual(_XMVECTOR3(aabb1.max), _XMVECTOR3(aabb2.max)) && DirectX::XMVector3LessOrEqual(_XMVECTOR3(aabb1.min), _XMVECTOR3(aabb2.min));
    bool aabb2_inside = DirectX::XMVector3GreaterOrEqual(_XMVECTOR3(aabb2.max), _XMVECTOR3(aabb1.max)) && DirectX::XMVector3LessOrEqual(_XMVECTOR3(aabb2.min), _XMVECTOR3(aabb1.min));

    if (aabb1_inside || aabb2_inside)
        return CollideType::INSIDE;

    return CollideType::INTERSECT;
}

CollideType reality::CapsuleToAABB(const AABBShape& aabb, const CapsuleShape& capsule)
{
    auto cap_to_aabb = CapsuleConvertAABB(capsule);
    return AABBtoAABB(cap_to_aabb, aabb);
}

CollideType reality::SphereToAABB(const SphereShape& sphere, const AABBShape& aabb)
{
    // Compute the squared distance between the sphere center and the AABB
    float sqDist = 0.0f;

    if (sphere.center.x < aabb.min.x) sqDist += (aabb.min.x - sphere.center.x) * (aabb.min.x - sphere.center.x);
    if (sphere.center.x > aabb.max.x) sqDist += (sphere.center.x - aabb.max.x) * (sphere.center.x - aabb.max.x);
    if (sphere.center.y < aabb.min.y) sqDist += (aabb.min.y - sphere.center.y) * (aabb.min.y - sphere.center.y);
    if (sphere.center.y > aabb.max.y) sqDist += (sphere.center.y - aabb.max.y) * (sphere.center.y - aabb.max.y);
    if (sphere.center.z < aabb.min.z) sqDist += (aabb.min.z - sphere.center.z) * (aabb.min.z - sphere.center.z);
    if (sphere.center.z > aabb.max.z) sqDist += (sphere.center.z - aabb.max.z) * (sphere.center.z - aabb.max.z);

    // If the squared distance is less than the squared radius, the sphere is intersecting or inside the AABB
    if (sqDist <= sphere.radius * sphere.radius) {
        // Check if the sphere is inside the AABB
        if (sphere.center.x > aabb.min.x + sphere.radius && sphere.center.x < aabb.max.x - sphere.radius &&
            sphere.center.y > aabb.min.y + sphere.radius && sphere.center.y < aabb.max.y - sphere.radius &&
            sphere.center.z > aabb.min.z + sphere.radius && sphere.center.z < aabb.max.z - sphere.radius) {
            return CollideType::INSIDE; // Inside
        }
        else {
            return CollideType::INTERSECT; // Intersecting
        }
    }
    else {
        return CollideType::OUTSIDE; // Outside
    }
}

CollideType reality::CapsuleToCapsule(const CapsuleShape& cap1, const CapsuleShape& cap2)
{
    vector<XMVECTOR> cap1_ab = { GetTipBaseAB(cap1)[2], GetTipBaseAB(cap1)[3] };
    vector<XMVECTOR> cap2_ab = { GetTipBaseAB(cap2)[2], GetTipBaseAB(cap2)[3] };

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

CollideType DLL_API reality::CapsuleToSphere(const CapsuleShape& cap, const SphereShape& sphere)
{
    auto capsule_info = GetTipBaseAB(cap);
    RayShape capsule_ray(capsule_info[2], capsule_info[3]);
    XMVECTOR closest_point = PointRaySegment(capsule_ray, _XMVECTOR3(sphere.center));

    if (Distance(closest_point, _XMVECTOR3(sphere.center)) < cap.radius + sphere.radius)
        return CollideType::INTERSECT;

    return CollideType::OUTSIDE;
}

CollideType reality::AABBToTriagnle(const AABBShape& aabb, const TriangleShape& triangle)
{
   // Check if all three vertices are inside the AABB
    int tri_vertices = 0;

    if (XMVector3GreaterOrEqual(_XMVECTOR3(triangle.vertex0), _XMVECTOR3(aabb.min)) && XMVector3LessOrEqual(_XMVECTOR3(triangle.vertex0), _XMVECTOR3(aabb.max)))
        tri_vertices++;

    if (XMVector3GreaterOrEqual(_XMVECTOR3(triangle.vertex1), _XMVECTOR3(aabb.min)) && XMVector3LessOrEqual(_XMVECTOR3(triangle.vertex1), _XMVECTOR3(aabb.max)))
        tri_vertices++;

    if (XMVector3GreaterOrEqual(_XMVECTOR3(triangle.vertex2), _XMVECTOR3(aabb.min)) && XMVector3LessOrEqual(_XMVECTOR3(triangle.vertex2), _XMVECTOR3(aabb.max)))
        tri_vertices++;

    if (tri_vertices == 3)
        return CollideType::INSIDE;

    if (tri_vertices > 0)
        return CollideType::INTERSECT;

    // Chect if 3 vertices are out of aabb but aabb corner intersecs with triangle.
    // it also can detect if aabb is completely inside of triangle 
    auto vertical_rays = GetYAxisRay(aabb);

    for (int i = 0; i < 4; ++i)
    {
        bool success = RayToTriangle(vertical_rays[i], triangle).success;

        if (success)
            return CollideType::INTERSECT;
    }

    // Check if 3 vertices are out of aabb and vertical rays are not intersect with triangle,
    // but edges of triangles intersect with aabb plane
    auto edge_rays = GetEdgeRays(triangle);
    auto aabb_tries = GetTriangles(aabb);

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

    BoundingBox box;
}

CapsuleCallback reality::CapsuleToTriangle(const CapsuleShape& cap, const TriangleShape& triangle)
{
    CapsuleCallback result;
    RayShape a_to_base(GetTipBaseAB(cap)[2], GetTipBaseAB(cap)[1]);

    auto raycallback = RayToTriangle(a_to_base, triangle);
    if (raycallback.success) // floor detected
    {
        result.reaction = CapsuleCallback::FLOOR;
        result.floor_pos = raycallback.point;
        return result;
    }

    // check if wall
    auto capsule_info = GetTipBaseAB(cap);

    XMVECTOR a_to_tri_normal = capsule_info[2] + (_XMVECTOR3(triangle.normal) * cap.radius * -1.0f);
    RayShape a_to_tri(capsule_info[2], a_to_tri_normal);

    XMVECTOR b_to_tri_normal = capsule_info[3] + (_XMVECTOR3(triangle.normal) * cap.radius * -1.0f);
    RayShape b_to_tri(capsule_info[3], a_to_tri_normal);

    auto a_raycallback = RayToTriangle(a_to_tri, triangle);
    auto b_raycallback = RayToTriangle(b_to_tri, triangle);

    if (a_raycallback.success || b_raycallback.success)
    {
        XMVECTOR a_to_base = GetTipBaseAB(cap)[1] - GetTipBaseAB(cap)[2];
        XMVECTOR a_to_tri_vector = a_to_tri_normal - capsule_info[2];
        float dot = XMVectorGetX(XMVector3Dot(a_to_base, a_to_tri_vector));
        if (dot <= 0.0001f)
        {
            result.reaction = CapsuleCallback::WALL;
            result.floor_pos = _XMVECTOR3(cap.base);
            return result;
        }
    }
    else
    {
        result.reaction = CapsuleCallback::NONE;
        result.floor_pos = _XMVECTOR3(cap.base);
        return result;
    }
}

XMVECTOR reality::PointRaySegment(const RayShape& ray, const XMVECTOR& point)
{
    XMVECTOR segment = _XMVECTOR3(ray.end) - _XMVECTOR3(ray.start);
    XMVECTOR diff = DirectX::XMVectorSubtract(point, _XMVECTOR3(ray.start));
    float t = XMVectorSaturate(XMVector3Dot(diff, segment) / XMVector3Dot(segment, segment)).m128_f32[0];

    return _XMVECTOR3(ray.start) + t * segment;
}
XMVECTOR reality::GetRayDirection(const RayShape& ray)
{
    return XMVector3Normalize(_XMVECTOR3(ray.end) - _XMVECTOR3(ray.start));
}

array<RayShape, 3> reality::GetEdgeRays(const TriangleShape& tri)
{
    RayShape edge0(_XMVECTOR3(tri.vertex0), _XMVECTOR3(tri.vertex1));
    RayShape edge1(_XMVECTOR3(tri.vertex1), _XMVECTOR3(tri.vertex2));
    RayShape edge2(_XMVECTOR3(tri.vertex2), _XMVECTOR3(tri.vertex0));

    return { edge0, edge1, edge2 };
}

XMVECTOR reality::GetMinXZ(const TriangleShape& tri)
{
    XMVECTOR min_xz = XMVectorZero();

    min_xz.m128_f32[0] = tri.vertex0.x;
    min_xz.m128_f32[0] = min(min_xz.m128_f32[0], tri.vertex1.x);
    min_xz.m128_f32[0] = min(min_xz.m128_f32[0], tri.vertex2.x);

    min_xz.m128_f32[2] = tri.vertex0.z;
    min_xz.m128_f32[2] = min(min_xz.m128_f32[2], tri.vertex1.z);
    min_xz.m128_f32[2] = min(min_xz.m128_f32[2], tri.vertex2.z);

    return min_xz;
}

XMVECTOR reality::GetMaxXZ(const TriangleShape& tri)
{
    XMVECTOR max_xz = XMVectorZero();

    max_xz.m128_f32[0] = tri.vertex0.x;
    max_xz.m128_f32[0] = max(max_xz.m128_f32[0], tri.vertex1.x);
    max_xz.m128_f32[0] = max(max_xz.m128_f32[0], tri.vertex2.x);

    max_xz.m128_f32[2] = tri.vertex0.z;
    max_xz.m128_f32[2] = max(max_xz.m128_f32[2], tri.vertex1.z);
    max_xz.m128_f32[2] = max(max_xz.m128_f32[2], tri.vertex2.z);

    return max_xz;
}


array<XMFLOAT3, 8> reality::GetCorners(const AABBShape& aabb)
{
    array<XMFLOAT3, 8> corners;
    corners[0] = XMFLOAT3(aabb.min.x, aabb.min.y, aabb.min.z);
    corners[1] = XMFLOAT3(aabb.min.x, aabb.min.y, aabb.max.z);
    corners[2] = XMFLOAT3(aabb.min.x, aabb.max.y, aabb.min.z);
    corners[3] = XMFLOAT3(aabb.min.x, aabb.max.y, aabb.max.z);
    corners[4] = XMFLOAT3(aabb.max.x, aabb.min.y, aabb.min.z);
    corners[5] = XMFLOAT3(aabb.max.x, aabb.min.y, aabb.max.z);
    corners[6] = XMFLOAT3(aabb.max.x, aabb.max.y, aabb.min.z);
    corners[7] = XMFLOAT3(aabb.max.x, aabb.max.y, aabb.max.z);

    return corners;
}

array<TriangleShape, 12> reality::GetTriangles(const AABBShape& aabb)
{
    XMFLOAT3* corners = GetCorners(aabb).data();
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

array<RayShape, 4> reality::GetYAxisRay(const AABBShape& aabb)
{
    XMFLOAT3* corners = GetCorners(aabb).data();
    array<RayShape, 4> y_ray;
    y_ray[0] = RayShape(_XMVECTOR3(corners[2]), _XMVECTOR3(corners[0]));
    y_ray[1] = RayShape(_XMVECTOR3(corners[6]), _XMVECTOR3(corners[4]));
    y_ray[2] = RayShape(_XMVECTOR3(corners[3]), _XMVECTOR3(corners[1]));
    y_ray[3] = RayShape(_XMVECTOR3(corners[7]), _XMVECTOR3(corners[5]));

    return y_ray;
}

array<XMVECTOR, 4> reality::GetTipBaseAB(const CapsuleShape& capsule)
{
    XMVECTOR base = _XMVECTOR3(capsule.base);
    XMVECTOR tip = base + XMVectorSet(0, capsule.height, 0, 0);

    XMVECTOR normal = XMVector3Normalize(tip - base);
    XMVECTOR lineend = normal * capsule.radius;
    XMVECTOR A = base + lineend;
    XMVECTOR B = tip - lineend;

    return { tip, base, A, B };
}

AABBShape reality::CapsuleConvertAABB(const CapsuleShape& capsule)
{
    XMVECTOR cap_min = _XMVECTOR3(capsule.base) + XMVectorSet(-capsule.radius, 0, -capsule.radius, 0);
    XMVECTOR cap_max = _XMVECTOR3(capsule.base) + XMVectorSet(capsule.radius, capsule.height, capsule.radius, 0);
    return AABBShape(_XMFLOAT3(cap_min), _XMFLOAT3(cap_max));
}

SphereShape reality::AABBTConvertSphere(const AABBShape& aabb)
{
    float aabb_radius = Distance(_XMVECTOR3(aabb.max), _XMVECTOR3(aabb.center));
    return SphereShape(aabb.center, aabb_radius);
}
