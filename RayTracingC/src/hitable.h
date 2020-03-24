#pragma once

#include "ray.h"

struct hit_record 
{
	float T;
	vec3 Pos;
	vec3 Normal;
};

class hitable
{
public:
	virtual bool hit(const ray& Ray, float TMin, float TMax, hit_record& Rec) const = 0;
};