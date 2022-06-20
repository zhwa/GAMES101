// Created by LEI XU on 5/16/19.
#pragma once
#include "Vector.hpp"

enum class MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

class Material
{
public:
    Material(MaterialType t = MaterialType::DIFFUSE_AND_GLOSSY, Vector3f c = Vector3f(1, 1, 1), Vector3f e = Vector3f(0, 0, 0)) : m_type(t), m_color(c), m_emission(e) {}
    MaterialType getType() { return m_type; }
    Vector3f getColor() { return m_color; }
    Vector3f getEmission() { return m_emission; }
    Vector3f getColorAt(double u, double v) { return Vector3f(); }

public:
    float ior;
    float Kd, Ks;
    float specularExponent;

private:
    MaterialType m_type;
    Vector3f m_color;
    Vector3f m_emission;
};