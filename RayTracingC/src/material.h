#pragma once

#include "random.h"
#include "vec3.h"
#include "hitable.h"
#include <stdlib.h>

vec3 randomInUnitSphere()
{
	vec3 P;
	do
	{
		P = 2.f * vec3(random_double(), random_double(), random_double()) - vec3(1, 1, 1);
	} while (P.squared_length() >= 1.f);

	return P;
}

vec3 reflect(const vec3& Vec, const vec3& Normal)
{
	return Vec - 2 * dot(Vec, Normal) * Normal;
}

bool refract(const vec3& Vec, const vec3& Normal, float NiOverNt, vec3& Refracted)
{
	vec3 UV = unit_vector(Vec);
	float Dt = dot(UV, Normal);
	float Discriminant = 1.f - NiOverNt * NiOverNt*(1 - Dt * Dt);
	if (Discriminant > 0)
	{
		Refracted = NiOverNt * (UV - Normal * Dt) - Normal * sqrt(Discriminant);
		return true;
	}

	return false;
}

float schlick(float Cos, float RefIndex)
{
	float R0 = (1 - RefIndex) / (1 + RefIndex);
	R0 = R0 * R0;
	return R0 + (1 - R0) * pow((1 - Cos), 5);
}



class material
{
public:
	virtual bool scatter(const ray& RayIn, const hit_record& Rec, vec3& Attenuation, ray& Scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(const vec3& A) : Albedo(A) {}
	virtual bool scatter(const ray& RayIn, const hit_record& Rec, vec3& Attenuation, ray& Scattered) const
	{
		vec3 Target = Rec.Pos + Rec.Normal + randomInUnitSphere();
		Scattered = ray(Rec.Pos, Target - Rec.Pos);
		Attenuation = Albedo;
		return true;
	}

	vec3 Albedo;
};

class metal : public material
{
public:
	metal(const vec3& A, float F) : Albedo(A) 
	{
		if (F < 1) Fuzz = F; else Fuzz = 1;
	}

	virtual bool scatter(const ray& RayIn, const hit_record& Rec, vec3& Attenuation, ray& Scattered) const
	{
		vec3 Reflected = reflect(unit_vector(RayIn.direction()), Rec.Normal);
		Scattered = ray(Rec.Pos, Reflected + Fuzz*randomInUnitSphere());
		Attenuation = Albedo;
		return (dot(Scattered.direction(), Rec.Normal) > 0);
	}

	vec3 Albedo;
	float Fuzz;
};

class dielectric : public material
{
public:
	dielectric(float Ri) : ReflectiveIndex(Ri) {};

	virtual bool scatter(const ray& RayIn, const hit_record& Rec, vec3& Attenuation, ray& Scattered) const
	{
		vec3 OutwardNormal;
		vec3 Reflected = reflect(RayIn.direction(), Rec.Normal);
		float NiOverNt;
		Attenuation = vec3(1.f, 1.f, 1.f);
		vec3 Refracted;
		float ReflectProb;
		float Cos;
		if (dot(RayIn.direction(), Rec.Normal) > 0)
		{
			OutwardNormal = -Rec.Normal;
			NiOverNt = ReflectiveIndex;
			Cos = ReflectiveIndex * dot(RayIn.direction(), Rec.Normal) / RayIn.direction().length();
		}
		else
		{
			OutwardNormal = Rec.Normal;
			NiOverNt = 1.f / ReflectiveIndex;
			Cos = -dot(RayIn.direction(), Rec.Normal) / RayIn.direction().length();
		}

		if (refract(RayIn.direction(), OutwardNormal, NiOverNt, Refracted))
		{
			Scattered = ray(Rec.Pos, Refracted);
			ReflectProb = schlick(Cos, ReflectiveIndex);
		}
		else
		{
			Scattered = ray(Rec.Pos, Reflected);
			ReflectProb = 1.f;
		}

		if (random_double() < ReflectProb)
		{
			Scattered = ray(Rec.Pos, Reflected);
		}
		else
		{
			Scattered = ray(Rec.Pos, Refracted);
		}

		return true;
	}

	float ReflectiveIndex;
};