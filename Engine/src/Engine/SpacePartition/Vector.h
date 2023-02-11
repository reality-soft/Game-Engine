#pragma once
#include <math.h>
#include <cstdlib>
#include <stdarg.h>

namespace KGCA41B {
#define T_PI 3.141592f
#define DegreeToRadian(x)  (x *(T_PI / 180.0f))
#define RadianToDegree(x) (x *(180.0f / T_PI))
#define T_Epsilon 0.001f

    template<size_t n>
    class Vector
    {
        friend class Vector;
    private:
        float elements[n];
    public:
        template<typename... type>
        Vector(type const &... elements) : elements{ static_cast<float const>(elements)... }
        {
            static_assert(not (n == 1), "A vector of size 1 cannot be created.");
        }

        Vector<n> operator *(float rhs) const
        {
            Vector<n> product = *this;

            for (size_t i = 0; i < n; ++i)
                product.elements[i] *= c;

            return product;
        }
        Vector<n>& operator *=(float c) {
            return *this = *this * c;
        }
        Vector<n> operator /(float c) {
            return *this * (1 / c);
        }
        Vector<n>& operator /=(float c) {
            return *this = *this / c;
        }

        Vector<n> operator +(void) const
        {
            return *this;
        }
        Vector<n> operator -(void) const
        {
            return *this * -1;
        }

        Vector<n> operator +(Vector<n> const& rhs) const
        {
            Vector<n> product = *this;

            for (size_t i = 0; i < n; ++i)
                product.elements[i] += rhs.elements[i];

            return product;
        }
        Vector<n>& operator +=(Vector<n> const& rhs)
        {
            return *this = *this + rhs;
        }
        Vector<n> operator -(Vector<n> const& rhs) const
        {
            return *this + (-rhs);
        }
        Vector<n>& operator -=(Vector<n> const& rhs)
        {
            return *this = *this - rhs;
        }

        bool operator ==(Vector<n> const& rhs) const
        {
            for (int i = 0; i < n; i++)
                if (fabs(elements[i] - rhs[i]) > T_Epsilon) return false;
            return true;
        }

        float& operator [](size_t index) const
        {
            return const_cast<float&>(elements[index]);
        }

        inline float Length() const
        {
            return sqrt(dot(*this, *this));
        }

        Vector<n>& Normalize()
        {
            return v / v.Length();
        }

        static inline Vector<n> GetRandomVector(int lower_bound, int upper_bound)
        {
            Vector<n> vector;
            for (int i = 0; i < n; i++) {
                vector[i] = rand() % (upper_bound + 1) + lower_bound;
            }
            return vector;
        }
    };

    template<size_t n>
    inline float dot(Vector<n> const& u, Vector<n> const& v)
    {
        float dotProduct = 0;

        for (int i = 0; i < n; i++)
            dotProduct += u[i] * v[i];

        return dotProduct;
    }
}