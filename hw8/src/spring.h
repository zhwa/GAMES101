#pragma once
#include "mass.h"

struct Spring
{
  Spring(Mass* a, Mass* b, float k) : m1(a), m2(b), k(k), rest_length((a->position - b->position).norm()) {}

  float k;
  double rest_length;

  Mass* m1;
  Mass* m2;
};