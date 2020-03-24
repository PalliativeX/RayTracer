#pragma once

#include "ray.h"

class camera
{
public:
	camera()
	{
		LowerLeftCorner = vec3(-2.f, -1.f, -1.f);
		Horizontal = vec3(4.f, 0.f, 0.f);
		Vertical = vec3(0.f, 2.f, 0.f);
		Origin = vec3(0.f, 0.f, 0.f);
	}
	ray getRay(float U, float V) 
	{ 
		return ray(Origin, LowerLeftCorner + U*Horizontal + V*Vertical - Origin); 
	}

	vec3 LowerLeftCorner;
	vec3 Horizontal;
	vec3 Vertical;
	vec3 Origin;
};