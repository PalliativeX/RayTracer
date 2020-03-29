#pragma once

#include "ray.h"
#include "random.h"

#define PI 3.141592653589793238

vec3 RandomInUnitDisk() 
{
	vec3 p;
	do 
	{
		p = 2.0*vec3(RandomDouble(), RandomDouble(), 0) - vec3(1, 1, 0);
	} while (Dot(p, p) >= 1.0);

	return p;
}

class camera 
{
public:
	camera(vec3 LookFrom, vec3 LookAt, vec3 VUp, float VFov, float Aspect, float Aperture, float FocusDist)
	{
		LensRadius = Aperture / 2;
		float Theta = VFov * PI / 180;
		float HalfHeight = tan(Theta / 2);
		float HalfWidth = Aspect * HalfHeight;
		Origin = LookFrom;
		W = UnitVector(LookFrom - LookAt);
		U = UnitVector(Cross(VUp, W));
		V = Cross(W, U);
		LowerLeftCorner = Origin - HalfWidth * FocusDist*U - HalfHeight * FocusDist*V - FocusDist * W;
		Horizontal = 2 * HalfWidth*FocusDist*U;
		Vertical = 2 * HalfHeight*FocusDist*V;
	}
	ray GetRay(float S, float T) 
	{
		vec3 Rd = LensRadius * RandomInUnitDisk();
		vec3 Offset = U * Rd.x() + V * Rd.y();
		return ray(Origin + Offset, LowerLeftCorner + S * Horizontal + T * Vertical - Origin - Offset);
	}

	vec3 LowerLeftCorner;
	vec3 Horizontal;
	vec3 Vertical;
	vec3 Origin;
	vec3 U, V, W;
	float LensRadius;
};
