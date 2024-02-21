#ifndef VEC_HPP
#define VEC_HPP


#include <cstdint>
#include <cmath>
#include <algorithm>
#include <ostream>
#include <format>


template<typename T, int D>
struct VecBase
{
    T e[D];

    friend std::ostream & operator<< <T, D> (std::ostream & os, const VecBase<T, D> & v);
};

template<typename T, int D>
std::ostream & operator<< (std::ostream & os, const VecBase<T, D> & v)
{
    os << '[';

    for (int i = 0; i < D; ++i) {
        os << std::format("{:.4}", reinterpret_cast<T *>(const_cast<VecBase<T, D> *>(&v))[i]);

        if (i != D - 1)
            os << ", ";
    }
    
    os << ']';

    return os;
}



template<typename T>
struct VecBase<T, 1>
{
    T w;
};

template<typename T>
struct VecBase<T, 2>
{
    VecBase() = default;
    VecBase(const T & e_1, const T &  e_2) : x(e_1), y(e_2) {}

    union { T x, w; };
    union { T y, a; };
};

template<typename T>
struct VecBase<T, 3>
{
    VecBase() = default;
    VecBase(const T & e_1, const T &  e_2, const T & e_3) : x(e_1), y(e_2), z(e_3) {}

    union { T x, r; };
    union { T y, g; };
    union { T z, b; };
};

template<typename T>
struct VecBase<T, 4>
{
    VecBase() = default;
    VecBase(const T & e_1, const T & e_2, const T & e_3, const T & e_4) : x(e_1), y(e_2), z(e_3), w(e_4) {}

    union { T x, r; };
    union { T y, g; };
    union { T z, b; };
    union { T w, a; };
};


// Declaration

template<typename T, int D>
struct Vec : public VecBase<T, D>
{
    Vec() = default;

    Vec(const T & e_1, const T & e_2) : VecBase<T, D>(e_1, e_2) {}
    Vec(const T & e_1, const T & e_2, const T & e_3) : VecBase<T, D>(e_1, e_2, e_3) {}
    Vec(const T & e_1, const T & e_2, const T & e_3, const T & e_4) : VecBase<T, D>(e_1, e_2, e_3, e_4) {}

    void operator*= (const T & c);
    void operator/= (const T & c);
    void operator+= (const T & c);
    void operator-= (const T & c);

    Vec<T, D> operator* (const T & c) const;
    Vec<T, D> operator/ (const T & c) const;
    Vec<T, D> operator+ (const T & c) const;
    Vec<T, D> operator- (const T & c) const;

    void operator*= (const Vec<T, D> & v);
    void operator/= (const Vec<T, D> & v);
    void operator+= (const Vec<T, D> & v);
    void operator-= (const Vec<T, D> & v);

    Vec<T, D> operator* (const Vec<T, D> & v) const;
    Vec<T, D> operator/ (const Vec<T, D> & v) const;
    Vec<T, D> operator+ (const Vec<T, D> & v) const;
    Vec<T, D> operator- (const Vec<T, D> & v) const;

    Vec<T, D> operator- () const;

    T norm() const;
    T norm_squared() const;

    Vec<T, D> unit() const;

    bool near_zero(const float ord = std::numeric_limits<float>::infinity(), const float s = 1e-8) const;
    
    Vec<T, D> reflect(const Vec<T, D> & n) const;
};

/*
template<typename T, int D>
struct Point : public VecBase<T, D>
{
    Point() = default;
    Point(const T & e_1, const T & e_2) : VecBase<T, D>(e_1, e_2) {}
    Point(const T & e_1, const T & e_2, const T & e_3) : VecBase<T, D>(e_1, e_2, e_3) {}
    Point(const T & e_1, const T & e_2, const T & e_3, const T & e_4) : VecBase<T, D>(e_1, e_2, e_3, e_4) {}

    void operator+= (const Vec<T, D> & v);
    void operator-= (const Vec<T, D> & v);

    Point<T, D> operator+ (const Vec<T, D> & v) const;
    Point<T, D> operator- (const Vec<T, D> & v) const;

    Vec<T, D> operator- (const Point<T, D> & p) const;
};
*/

template<typename T, int D>
struct Color : public Vec<T, D>
{
    Color() = default;
    Color(const Vec<T, D> & v) : Vec<T, D>(v) {}

    Color(const T & e_1, const T & e_2) : Vec<T, D>(e_1, e_2) {}
    Color(const T & e_1, const T & e_2, const T & e_3) : Vec<T, D>(e_1, e_2, e_3) {}
    Color(const T & e_1, const T & e_2, const T & e_3, const T & e_4) : Vec<T, D>(e_1, e_2, e_3, e_4) {}

    explicit operator Color<uint8_t, D> ();
    explicit operator Color<float, D> ();

    Color<T, D> sqrt() const;
};


template<typename T, int D>
Color<T, D>::operator Color<uint8_t, D> ()
{
    Color<uint8_t, D> v;

    if constexpr (std::is_floating_point<T>::value)
        for (int i = 0; i < D; ++i)
            reinterpret_cast<uint8_t *>(&v)[i] = 
                static_cast<uint8_t>(std::clamp(reinterpret_cast<T *>(this)[i], T(0), T(1)) * 255);
    else
        v = *this;
            
    return v;
}

template<typename T, int D>
Color<T, D>::operator Color<float, D> ()
{
    Color<float, D> v;

    if constexpr (std::is_floating_point<T>::value)
        for (int i = 0; i < D; ++i)
            reinterpret_cast<float *>(&v)[i] = static_cast<float>(reinterpret_cast<T *>(this)[i]);
    else if constexpr (std::is_integral<T>::value)
        for (int i = 0; i < D; ++i)
            reinterpret_cast<float *>(&v)[i] = static_cast<float>(reinterpret_cast<T *>(this)[i]) / 255;
    else
        v = *this;
            
    return v;
}


// Useful links

// template<typename T, int D>
// using Color = Vec<T, D>;

using ColorRGB = Color<float, 3>;
using ColorRGBA = Color<float, 4>;

using Vec2 = Vec<float, 2>;
using Vec3 = Vec<float, 3>;
using Vec4 = Vec<float, 4>;

using Point2 = Vec<float, 2>;
using Point3 = Vec<float, 3>;


// Defintion

template<typename T, int D>
inline void Vec<T, D>::operator*= (const T & v) { for (int i = 0; i < D; ++i) reinterpret_cast<T *>(this)[i] *= v; }

template<typename T, int D>
inline void Vec<T, D>::operator/= (const T & v) { for (int i = 0; i < D; ++i) reinterpret_cast<T *>(this)[i] /= v; }

template<typename T, int D>
inline void Vec<T, D>::operator+= (const T & v) { for (int i = 0; i < D; ++i) reinterpret_cast<T *>(this)[i] += v; }

template<typename T, int D>
inline void Vec<T, D>::operator-= (const T & v) { for (int i = 0; i < D; ++i) reinterpret_cast<T *>(this)[i] -= v; }


template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator* (const T & v) const { Vec<T, D> u(*this); u *= v; return u; }

template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator/ (const T & v) const { Vec<T, D> u(*this); u /= v; return u; }

template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator+ (const T & v) const { Vec<T, D> u(*this); u += v; return u; }

template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator- (const T & v) const { Vec<T, D> u(*this); u -= v; return u; }


template <typename T, typename U, int D>
inline Vec<T, D> operator* (const U & c, Vec<T, D> v) { Vec<T, D> u(v); u *= c; return u; }

template <typename T, typename U, int D>
inline Vec<T, D> operator/ (const U & c, Vec<T, D> v) { Vec<T, D> u(v); u /= c; return u; }

template <typename T, typename U, int D>
inline Vec<T, D> operator+ (const U & c, Vec<T, D> v) { Vec<T, D> u(v); u += c; return u; }

template <typename T, typename U, int D>
inline Vec<T, D> operator- (const U & c, Vec<T, D> v) { Vec<T, D> u(v); u -= c; return u; }


template <typename T, int D>
inline void Vec<T, D>::operator*= (const Vec<T, D> & v)
{
    for (int i = 0; i < D; ++i)
        reinterpret_cast<T *>(this)[i] *= reinterpret_cast<T *>(const_cast<Vec<T, D> *>(&v))[i];
}

template <typename T, int D>
inline void Vec<T, D>::operator/= (const Vec<T, D> & v)
{
    for (int i = 0; i < D; ++i)
        reinterpret_cast<T *>(this)[i] /= reinterpret_cast<T *>(const_cast<Vec<T, D> *>(&v))[i];
}

template <typename T, int D>
inline void Vec<T, D>::operator+= (const Vec<T, D> & v)
{
    for (int i = 0; i < D; ++i)
        reinterpret_cast<T *>(this)[i] += reinterpret_cast<T *>(const_cast<Vec<T, D> *>(&v))[i];
}

template <typename T, int D>
inline void Vec<T, D>::operator-= (const Vec<T, D> & v)
{
    for (int i = 0; i < D; ++i)
        reinterpret_cast<T *>(this)[i] -= reinterpret_cast<T *>(const_cast<Vec<T, D> *>(&v))[i];
}


template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator* (const Vec<T, D> & v) const { Vec<T, D> u = (*this); u *= v; return u; }

template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator/ (const Vec<T, D> & v) const { Vec<T, D> u = (*this); u /= v; return u; }

template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator+ (const Vec<T, D> & v) const { Vec<T, D> u = (*this); u += v; return u; }

template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator- (const Vec<T, D> & v) const { Vec<T, D> u = (*this); u -= v; return u; }


template <typename T, int D>
inline Vec<T, D> Vec<T, D>::operator- () const
{
    Vec<T, D> u = (*this);
    
    for (int i = 0; i < D; ++i)
        reinterpret_cast<T *>(&u)[i] = -reinterpret_cast<T *>(&u)[i];

    return u;
}

template <typename T, int D>
inline T Vec<T, D>::norm() const
{
    T p = 0;

    for (int i = 0; i < D; ++i)
        p += (
            reinterpret_cast<T *>(const_cast<Vec<T, D> *>(this))[i] *
            reinterpret_cast<T *>(const_cast<Vec<T, D> *>(this))[i]
        );

    return std::sqrt(p);
};

template <typename T, int D>
inline T Vec<T, D>::norm_squared() const
{
    T p = 0;

    for (int i = 0; i < D; ++i)
        p += (
            reinterpret_cast<T *>(const_cast<Vec<T, D> *>(this))[i] *
            reinterpret_cast<T *>(const_cast<Vec<T, D> *>(this))[i]
        );

    return p;
};


template <typename T, int D>
inline Vec<T, D> Vec<T, D>::unit() const
{
    return (*this) / (*this).norm();
}

template <typename T, int D>
inline Color<T, D> Color<T, D>::sqrt() const
{
    Color<T, D> v;

    for (int i = 0; i < D; ++i)
        reinterpret_cast<T *>(&v)[i] = std::sqrt(reinterpret_cast<T *>(const_cast<Color<T, D> *>(this))[i]);

    return v;
}

template <typename T, int D>
inline bool Vec<T, D>::near_zero(const float ord, const float s) const
{
    // Note: implementation only for `\ell_\infty` norm
    for (int i = 0; i < D; ++i)
        if (std::abs(reinterpret_cast<T *>(const_cast<Vec<T, D> *>(this))[i]) >= s)
            return false;

    return true;
}


// Functions

template <typename T, int D>
inline T norm(const Vec<T, D> & v) { return v.norm(); }

template <typename T, int D>
inline Vec<T, D> unit(const Vec<T, D> & v) { return v.unit(); }

template <typename T, int D>
inline Color<T, D> sqrt(const Color<T, D> & v) { return v.sqrt(); }

template <typename T, int D>
inline T dot(const Vec<T, D> & u, const Vec<T, D> & v)
{
    T p = 0;

    for (int i = 0; i < D; ++i)
        p += (
            reinterpret_cast<T *>(const_cast<Vec<T, D> *>(&u))[i] *
            reinterpret_cast<T *>(const_cast<Vec<T, D> *>(&v))[i]
        );

    return p;
}

inline Vec3 cross(const Vec3 & u, const Vec3 & v)
{
    return { u.y * v.z - v.y * u.z, v.x * u.z - u.x * v.z, u.x * v.y - v.x * u.y };
}

template <typename T, int D>
inline Vec<T, D> Vec<T, D>::reflect(const Vec<T, D> & n) const
{
    // n is a unit vector, v can be arbitrary
    return (*this) - 2 * dot((*this), n) * n;
}


#endif // VEC_HPP