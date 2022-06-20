// Created by LEI XU on 5/16/19.
#pragma once
#include "Ray.hpp"
#include "Vector.hpp"
#include <limits>
#include <array>
#include <cmath>

struct Bounds3
{
    Vector3f pMin{ Vector3f(std::numeric_limits<float>::lowest()) }, pMax{ Vector3f(std::numeric_limits<float>::max()) }; // two points to specify the bounding box

    Bounds3() = default;
    Bounds3(const Vector3f& p) : pMin(p), pMax(p) {}
    Bounds3(const Vector3f& p1, const Vector3f& p2) : pMin(Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z))), pMax(Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z))) {}

    Vector3f Diagonal() const noexcept { return pMax - pMin; }

    int maxExtent() const noexcept
    {
        Vector3f d = Diagonal();
        if (d.x > d.y && d.x > d.z)
        {
            return 0;
        }
        if (d.y > d.z)
        {
            return 1;
        }
        return 2;
    }

    double SurfaceArea() const noexcept
    {
        Vector3f d = Diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    Vector3f Centroid() const noexcept { return 0.5 * pMin + 0.5 * pMax; }

    Bounds3 Intersect(const Bounds3& b) const noexcept
    {
        return Bounds3(Vector3f(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y), fmax(pMin.z, b.pMin.z)), Vector3f(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y), fmin(pMax.z, b.pMax.z)));
    }

    Vector3f Offset(const Vector3f& p) const noexcept
    {
        Vector3f o = p - pMin;
        if (pMax.x > pMin.x)
        {
            o.x /= pMax.x - pMin.x;
        }
        if (pMax.y > pMin.y)
        {
            o.y /= pMax.y - pMin.y;
        }
        if (pMax.z > pMin.z)
        {
            o.z /= pMax.z - pMin.z;
        }
        return o;
    }

    bool Overlaps(const Bounds3& b1, const Bounds3& b2) const noexcept
    {
        bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
        bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
        bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
        return (x && y && z);
    }

    bool Inside(const Vector3f& p, const Bounds3& b) const noexcept
    {
        return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y && p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
    }

    const Vector3f& operator[](int i) const noexcept
    {
        return i == 0 ? pMin : pMax;
    }

    inline bool IntersectP(const Ray& ray, const Vector3f& invDir) const
    {
        // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), use this because Multiply is faster than Division
        // TODO test if ray bound intersects
        float t_enter{ std::numeric_limits<float>::lowest() };
        float t_exit{ std::numeric_limits<float>::max() };
        if (ray.direction.x > 0)
        {
            t_enter = std::max(t_enter, (pMin.x - ray.origin.x) * ray.direction_inv.x);
            t_exit = std::min(t_exit, (pMax.x - ray.origin.x) * ray.direction_inv.x);
        }
        else
        {
            t_enter = std::max(t_enter, (pMax.x - ray.origin.x) * ray.direction_inv.x);
            t_exit = std::min(t_exit, (pMin.x - ray.origin.x) * ray.direction_inv.x);
        }
        if (ray.direction.y > 0)
        {
            t_enter = std::max(t_enter, (pMin.y - ray.origin.y) * ray.direction_inv.y);
            t_exit = std::min(t_exit, (pMax.y - ray.origin.y) * ray.direction_inv.y);
        }
        else
        {
            t_enter = std::max(t_enter, (pMax.y - ray.origin.y) * ray.direction_inv.y);
            t_exit = std::min(t_exit, (pMin.y - ray.origin.y) * ray.direction_inv.y);
        }
        if (ray.direction.z > 0)
        {
            t_enter = std::max(t_enter, (pMin.z - ray.origin.z) * ray.direction_inv.z);
            t_exit = std::min(t_exit, (pMax.z - ray.origin.z) * ray.direction_inv.z);
        }
        else
        {
            t_enter = std::max(t_enter, (pMax.z - ray.origin.z) * ray.direction_inv.z);
            t_exit = std::min(t_exit, (pMin.z - ray.origin.z) * ray.direction_inv.z);
        }
        return t_enter <= t_exit && t_exit >= 0;
    }
};

inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b1.pMin, b2.pMin);
    ret.pMax = Vector3f::Max(b1.pMax, b2.pMax);
    return ret;
}

inline Bounds3 Union(const Bounds3& b, const Vector3f& p)
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b.pMin, p);
    ret.pMax = Vector3f::Max(b.pMax, p);
    return ret;
}