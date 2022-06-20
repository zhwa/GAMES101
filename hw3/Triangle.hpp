// Created by LEI XU on 4/11/19.
#pragma once
#include "Texture.hpp"
#include <Eigen/Eigen>
#include <array>

class Triangle
{
public:
    Eigen::Vector4f v[3]; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/
    Eigen::Vector3f color[3]; //color at each vertex;
    Eigen::Vector2f tex_coords[3]; //texture u,v
    Eigen::Vector3f normal[3]; //normal vector for each vertex

    Texture* tex{ nullptr };
    Triangle();

    Eigen::Vector4f a() const { return v[0]; }
    Eigen::Vector4f b() const { return v[1]; }
    Eigen::Vector4f c() const { return v[2]; }

    void setVertex(int ind, Eigen::Vector4f ver); /*set i-th vertex coordinates */
    void setNormal(int ind, Eigen::Vector3f n); /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/

    void setNormals(const std::array<Eigen::Vector3f, 3>& normals);
    void setColors(const std::array<Eigen::Vector3f, 3>& colors);
    void setTexCoord(int ind, Eigen::Vector2f uv ); /*set i-th vertex texture coordinate*/
    std::array<Eigen::Vector4f, 3> toVector4() const;
};