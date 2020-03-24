#pragma once

#include "hitable.h"

class sphere : public hitable
{
public:
	sphere() {}
	sphere(vec3 Cen, float R) : Center(Cen), Radius(R) {};
	virtual bool hit(const ray& Ray, float TMin, float TMax, hit_record& Rec) const;
	
	vec3 Center;
	float Radius;
};

bool sphere::hit(const ray& Ray, float TMin, float TMax, hit_record& Rec) const
{
	vec3 oc = Ray.origin() - Center;
    float A = dot(Ray.direction(), Ray.direction());
    float B = dot(oc, Ray.direction());
    float C = dot(oc, oc) - Radius*Radius;
    float Discriminant = B*B - A*C;
    if (Discriminant > 0) {
        float Temp = (-B - sqrt(Discriminant))/A;
        if (Temp < TMax && Temp > TMin) {
			Rec.T = Temp;
			Rec.Pos = Ray.point_at_parameter(Rec.T);
			Rec.Normal = (Rec.Pos - Center) / Radius;
            return true;
        }
        Temp = (-B + sqrt(Discriminant)) / A;
        if (Temp < TMax && Temp > TMin) {
			Rec.T = Temp;
			Rec.Pos = Ray.point_at_parameter(Rec.T);
			Rec.Normal = (Rec.Pos - Center) / Radius;
			return true;
        }
    }

    return false;
}