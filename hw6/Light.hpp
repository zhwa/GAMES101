// Created by Göksu Güvendiren on 2019-05-14.
#pragma once
#include "Vector.hpp"

struct Light
{
    Light(const Vector3f& p, const Vector3f& i) : position(p), intensity(i) {}
    virtual ~Light() = default;
    Vector3f position;
    Vector3f intensity;
};