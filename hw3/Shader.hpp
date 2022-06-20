// Created by LEI XU on 4/27/19.
#pragma once
#include "Texture.hpp"
#include <Eigen/Eigen>

struct fragment_shader_payload
{
    fragment_shader_payload() = default;
    fragment_shader_payload(const Eigen::Vector3f& col, const Eigen::Vector3f& nor,const Eigen::Vector2f& tc, Texture* tex) : color(col), normal(nor), tex_coords(tc), texture(tex) {}

    Eigen::Vector3f view_pos;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f tex_coords;
    Texture* texture{ nullptr };
};

struct vertex_shader_payload
{
    Eigen::Vector3f position;
};