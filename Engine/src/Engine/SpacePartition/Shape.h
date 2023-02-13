#include"stdafx.h"
#include"Vector.h"

namespace KGCA41B {
    template<size_t Dimension>
    class AABB
    {
    private:
        Vector<Dimension> min_coord_;
        Vector<Dimension> size_;
    public:
        bool operator == (AABB<Dimension>& dest)
        {
            if (min_coord_ == dest.min_coord_)
            {
                if (size_ == dest.size_)
                {
                    return true;
                }
            }
            return false;
        }
        AABB() {}
        AABB(const Vector<Dimension>& min_coord, const Vector<Dimension>& size)
        {
            Set(min_coord, size);
        }
        void Set(const Vector<Dimension>& min_coord, const Vector<Dimension>& size)
        {
            min_coord_ = min_coord;
            size_ = size;
        }
        AABB<Dimension>& operator=(const AABB<Dimension>& rhs) {
            min_coord_ = rhs.MinCoord();
            size_ = rhs.Size();
            return *this;
        }

        Vector<Dimension> MinCoord() const {
            return min_coord_;
        }
        Vector<Dimension> Size() const {
            return size_;
        }
        Vector<Dimension> MaxCoord() const {
            return min_coord_ + size_;
        }
        Vector<Dimension> CenterCoord() const {
            return (min_coord_ + size_) / 2;
        }
    };

    template<size_t Dimension>
    class nSphere
    {
    private:
        Vector<Dimension> center_;
        float radius_;
    public:
        nSphere() 
        {
            for (int i = 0; i < Dimension; i++)
                center_[i] = 0;
            radius_ = 1;
        };
        nSphere(const Vector<Dimension>& center, float radius) 
        {
            center_ = center;
            radius_ = radius;
        };
        void Set(const Vector<Dimension>& center, float radius)
        {
            center_ = center;
            radius_ = radius;
        }
        void Set(const AABB<Dimension>& cube) 
        {
            center_ = cube.CenterCoord();
            radius_ = (cube.size_ * 0.5f).length();
        }
        Vector<Dimension> Center() const {
            return center_;
        }
        float Radius() const {
            return radius_;
        }
    };

}
