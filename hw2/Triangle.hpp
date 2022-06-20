// Created by LEI XU on 4/11/19.
#pragma once
#include <Eigen/Eigen>
#include <array>

class Triangle
{
public:
    Eigen::Vector3f v[3]; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/
    Eigen::Vector3f color[3]; //color at each vertex;
    Eigen::Vector2f tex_coords[3]; //texture u,v
    Eigen::Vector3f normal[3]; //normal vector for each vertex

    //Texture *tex;
    Triangle();

    void setVertex(int ind, Eigen::Vector3f ver); /*set i-th vertex coordinates */
    void setNormal(int ind, Eigen::Vector3f n); /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/
    Eigen::Vector3f getColor() const { return color[0]*255; } // Only one color per triangle.
    void setTexCoord(int ind, float s, float t); /*set i-th vertex texture coordinate*/
    std::array<Eigen::Vector4f, 3> toVector4() const;
};