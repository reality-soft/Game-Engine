#pragma once
#include "Shape.h"

namespace reality {
    enum class CollisionType
    {
        C_OUT = 0,
        C_R_IN_L,
        C_L_IN_R,
        C_OVERLAP,
    };

    template<size_t Dimension>
    class Collision
    {
    public:
        static CollisionType CubeToCube(AABB<Dimension>& lhs, AABB<Dimension>& rhs)
        {
            float min_num;    float max_num;
            Vector<Dimension> lhs_max_coord = lhs.MaxCoord();
            Vector<Dimension> rhs_max_coord = rhs.MaxCoord();
            for (int i = 0; i < Dimension; i++) {
                min_num = lhs.MinCoord()[i] < rhs.MinCoord()[i] ? lhs.MinCoord()[i] : rhs.MinCoord()[i];
                max_num = lhs_max_coord[i] > rhs_max_coord[i] ? lhs_max_coord[i] : rhs_max_coord[i];
                if ((lhs.Size()[i] + rhs.Size()[i]) < (max_num - min_num))
                    return CollisionType::C_OUT;
            }

            Vector<Dimension> min_coord, max_coord;
            AABB<Dimension> intersect;

            for (int i = 0; i < Dimension; i++) {
                min_coord[i] = lhs.MinCoord()[i] > rhs.MinCoord()[i] ? lhs.MinCoord()[i] : rhs.MinCoord()[i];
                max_coord[i] = lhs_max_coord[i] < rhs_max_coord[i] ? lhs_max_coord[i] : rhs_max_coord[i];
            }

            intersect.Set(min_coord, max_coord - min_coord);
            if (intersect == lhs)
                return CollisionType::C_L_IN_R;
            if (intersect == rhs)
                return CollisionType::C_R_IN_L;
            return CollisionType::C_OVERLAP;
        }
        static CollisionType   SphereToSphere(nSphere<Dimension>& lhs, nSphere<Dimension>& rhs) {
            float sum_radius = lhs.Radius() + rhs.Radius();
            float sub_radius = fabs(lhs.Radius() - rhs.Radius());
            Vector<Dimension> vDir = lhs.Center() - rhs.Center();
            float distance = vDir.Length();

            if (sum_radius < distance)
            {
                return CollisionType::C_OUT;
            }
            else if (sub_radius < distance && distance <= sum_radius)
            {
                return CollisionType::C_OVERLAP;
            }
            else {
                if (lhs.Radius() > rhs.Radius()) return CollisionType::C_R_IN_L;
                else                             return CollisionType::C_L_IN_R;
            }
        }
    };

}