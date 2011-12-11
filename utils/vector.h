#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

typedef unsigned int uint;

//represents a vector of size N consisting of elements of type T
template <typename T, uint N>
class Vector {
public:
    /*Vector() {
        this->zero();
        init(0,0,0,0);
    }*/

    Vector(const Vector<T,N> &v)
        : x(N >= 1 ? data[0] : voidRef), y(N >= 1 ? data[1] : voidRef),
          z(N >= 1 ? data[2] : voidRef), w(N >= 1 ? data[3] : voidRef)
    {
        init(v.x, v.y, v.z, v.w);
        for (uint i = 4; i < N; i++)
            this->data[i] = v.data[i];

    }
/*
    Vector(T x, T y)
        : x(data[0]), y(data[0]), z(voidRef), w(voidRef)
    {
        this->zero();
        Vector(x,y,0,0);
        init(x,y,0,0);
    }

    Vector(T x, T y, T z)
        : x(data[0]), y(data[0]), z(data[2])
    {
        this->zero();
        init(x,y,z,0);
    }
*/
    Vector(T x=0, T y=0, T z=0, T w=0)
        : x(N >= 1 ? data[0] : voidRef), y(N >= 1 ? data[1] : voidRef),
          z(N >= 1 ? data[2] : voidRef), w(N >= 1 ? data[3] : voidRef)
    {
        this->zero();
        init(x,y,z,w);
    }

    void init(T x, T y, T z, T w) {
        voidRef = 0;
        if (N >= 1) data[0] = x;
        if (N >= 2) data[1] = y;
        if (N >= 3) data[2] = z;
        if (N >= 4) data[3] = w;
    }

    //clears all elements to 0
    void zero() {
        for (uint i = 0; i < N; i++)
            this->data[i] = 0;
    }

    //returns the magnitude (length) of the vector
    double magnitude() const {
        T sum = 0;
        for (uint i = 0; i < N; i++) {
            sum += data[i]*data[i];
        }

        return sqrt(sum);
    }

    //returns the unit vector of this vector
    Vector<T,N> unit() {
        Vector<T,N> result;
        double mag = magnitude();
        for (uint i = 0; i < N; i++) {
            result[i] = data[i] / mag;
        }
        return result;
    }

    //returns the dot product with another vector
    T dot(const Vector<T,N> &v) const {
        T result = 0;
        for (uint i = 0; i < N; i++) {
            result += data[i] * v.data[i];
        }

        return result;
    }

    //returns the cross product with another vector
    Vector<T,N> cross(const Vector<T,N> &v) const {
        Vector<T,N> c;
        const T (&a)[N] = data;
        const T (&b)[N] = v.data;

        switch(N) {
        case 1:
            c[0] = a[0]*b[0];
            break;
        case 2:
            c[0] = a[1]*b[0] - a[0]*b[1];
            c[1] = a[0]*b[0] - a[0]*b[0];
            break;
        case 3:
            c[0] = a[1]*b[2] - a[2]*b[1];
            c[1] = a[2]*b[0] - a[0]*b[2];
            c[2] = a[0]*b[1] - a[1]*b[0];
        default:
            //TODO: implement general cross product function
            break;
        }

        return c;
    }

    //const accessor
    T get(uint i) const {
        if (i < N)
            return data[i];
        return 0;
    }

    //vector subtraction
    Vector<T,N> operator-(const Vector<T,N> &v) const {
        Vector<T,N> result;
        for (uint i = 0; i < N; i++)
            result.data[i] = data[i] - v.data[i];
        return result;
    }

    //vector addition
    Vector<T,N> operator+(const Vector<T,N> &v) const {
        Vector<T,N> result;
        for (uint i = 0; i < N; i++)
            result.data[i] = data[i] + v.data[i];
        return result;
    }

    //vector scalar multiplication
    Vector<T,N> operator*(T scalar) {
        Vector<T,N> result;
        for (uint i = 0; i < N; i++)
            result.data[i] = data[i] * scalar;
        return result;
    }

    //vector scalar division
    Vector<T,N> operator/(T scalar) {
        Vector<T,N> result;
        for (uint i = 0; i < N; i++)
            result.data[i] = data[i] / scalar;
        return result;
    }

    //vector assignment
    Vector<T,N>& operator=(const Vector<T,N> &v) {
        if (this != &v) {
            for (uint i = 0; i < N; i++)
                data[i] = v.data[i];
        }
        return *this;
    }

    //vector element-wise comparison
    bool operator==(const Vector<T,N> &v) const {
        for (uint i = 0; i < N; i++)
            if (data[i] != v.data[i])
                return false;
        return true;
    }

    //vector not equals
    bool operator!=(const Vector<T,N> &v) const {
        return !operator==(v);
    }

    //lexicographically less than
    bool operator<(const Vector<T,N> &v) const {
        for (uint i = 0; i < N; i++) {
            if (data[i] == v.data[i]) continue;
            return data[i] < v.data[i];
        }

        return false;
    }

    T& operator[](uint i) {
        return this->data[i];
    }

public:
    //convenience references to the 1st, 2nd, 3rd, 4th elements respetively
    T &x;
    T &y;
    T &z;
    T &w;

protected:
    T data[N];
    T voidRef; //referenced by x,y,z, or w if N is less than 4
};

//typedefs for convenience
typedef Vector<int, 2> Vector2i;
typedef Vector<float, 2> Vector2f;
typedef Vector<double, 2> Vector2d;

typedef Vector<int,3 > Vector3i;
typedef Vector<float, 3> Vector3f;
typedef Vector<double, 3> Vector3d;

typedef Vector<int, 4> Vector4i;
typedef Vector<float, 4> Vector4f;
typedef Vector<double, 4> Vector4d;

#endif // VECTOR_H
