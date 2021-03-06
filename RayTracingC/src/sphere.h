#pragma once

#include "hitable.h"

class sphere : public hitable
{
public:
	sphere() {}
	sphere(vec3 Cen, float R, material *Mat) : Center(Cen), Radius(R), MatPtr(Mat) {};
	virtual bool Hit(const ray& Ray, float TMin, float TMax, hit_record& Rec) const;
	
	vec3 Center;
	float Radius;
	material *MatPtr;
};

bool sphere::Hit(const ray& Ray, float TMin, float TMax, hit_record& Rec) const
{
	vec3 oc = Ray.origin() - Center;
    float A = Dot(Ray.direction(), Ray.direction());
    float B = Dot(oc, Ray.direction());
    float C = Dot(oc, oc) - Radius*Radius;
    float Discriminant = B*B - A*C;
    if (Discriminant > 0) {
        float Temp = (-B - sqrt(Discriminant))/A;
        if (Temp < TMax && Temp > TMin) {
			Rec.T = Temp;
			Rec.Pos = Ray.point_at_parameter(Rec.T);
			Rec.Normal = (Rec.Pos - Center) / Radius;
			Rec.MatPtr = MatPtr;
            return true;
        }
        Temp = (-B + sqrt(Discriminant)) / A;
        if (Temp < TMax && Temp > TMin) {
			Rec.T = Temp;
			Rec.Pos = Ray.point_at_parameter(Rec.T);
			Rec.Normal = (Rec.Pos - Center) / Radius;
			Rec.MatPtr = MatPtr;
			return true;
        }
    }

    return false;
}