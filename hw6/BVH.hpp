// Created by LEI XU on 5/16/19.
#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <ctime>
#include "Object.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"
#include "Intersection.hpp"
#include "Vector.hpp"

struct BVHBuildNode
{
    Bounds3 bounds{ Bounds3() };
    BVHBuildNode* left{ nullptr };
    BVHBuildNode* right{ nullptr };
    Object* object{ nullptr };
    int splitAxis{ 0 };
    int firstPrimOffset{ 0 };
    int nPrimitives{ 0 };
};

class BVHAccel
{
public:
    // BVHAccel Public Types
    enum class SplitMethod { NAIVE, SAH };

    // BVHAccel Public Methods
    BVHAccel(std::vector<Object*> p, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::NAIVE);
    Intersection Intersect(const Ray& ray) const;
    Intersection getIntersection(BVHBuildNode* node, const Ray& ray)const;

private:
    // BVHAccel Private Methods
    BVHBuildNode* recursiveBuild(std::vector<Object*>objects);

private:
    // BVHAccel Private Data
    BVHBuildNode* root { nullptr };
    const int maxPrimsInNode{ 1 };
    const SplitMethod splitMethod{ SplitMethod::NAIVE };
    std::vector<Object*> primitives;
};