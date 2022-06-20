// Created by LEI XU on 5/16/19.
#pragma once
#include "Vector.hpp"
#include "Material.hpp"

class Object;

struct Intersection
{
	bool happened{ false };
	Vector3f coords{ Vector3f() };
	Vector3f normal{ Vector3f() };
	double distance{ std::numeric_limits<double>::max() };
	Object* obj{ nullptr };
	Material* m{ nullptr };
};