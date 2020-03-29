#pragma once

#include "random.h"
#include "vec3.h"
#include "hitable.h"
#include <stdlib.h>

vec3 RandomInUnitSphere()
{
	vec3 P;
	do
	{
		P = 2.f * vec3(RandomDouble(), RandomDouble(), RandomDouble()) - vec3(1, 1, 1);
	} while (P.SquaredLength() >= 1.f);

	return P;
}

vec3 Reflect(const vec3& Vec, const vec3& Normal)
{
	return Vec - 2 * Dot(Vec, Normal) * Normal;
}

bool Refract(const vec3& Vec, const vec3& Normal, float NiOverNt, vec3& Refracted)
{
	vec3 UV = UnitVector(Vec);
	float Dt = Dot(UV, Normal);
	float Discriminant = 1.f - NiOverNt * NiOverNt*(1 - Dt * Dt);
	if (Discriminant > 0)
	{
		Refracted = NiOverNt * (UV - Normal * Dt) - Normal * sqrt(Discriminant);
		return true;
	}

	return false;
}

float Schlick(float Cos, float RefIndex)
{
	float R0 = (1 - RefIndex) / (1 + RefIndex);
	R0 = R0 * R0;
	return R0 + (1 - R0) * pow((1 - Cos), 5);
}



class material
{
public:
	virtual bool Scatter(const ray& RayIn, const hit_record& Rec, vec3& Attenuation, ray& Scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(const vec3& A) : Albedo(A) {}
	virtual bool Scatter(const ray& RayIn, const hit_record& Rec, vec3& Attenuation, ray& Scattered) const
	{
		vec3 Target = Rec.Pos + Rec.Normal + RandomInUnitSphere();
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

	virtual bool Scatter(const ray& RayIn, const hit_record& Rec, vec3& Attenuation, ray& Scattered) const
	{
		vec3 Reflected = Reflect(UnitVector(RayIn.direction()), Rec.Normal);
		Scattered = ray(Rec.Pos, Reflected + Fuzz*RandomInUnitSphere());
		Attenuation = Albedo;
		return (Dot(Scattered.direction(), Rec.Normal) > 0);
	}

	vec3 Albedo;
	float Fuzz;
};

class dielectric : public material
{
public:
	dielectric(float Ri) : ReflectiveIndex(Ri) {};

	virtual bool Scatter(const ray& RayIn, const hit_record& Rec, vec3& Attenuation, ray& Scattered) const
	{
		vec3 OutwardNormal;
		vec3 Reflected = Reflect(RayIn.direction(), Rec.Normal);
		float NiOverNt;
		Attenuation = vec3(1.f, 1.f, 1.f);
		vec3 Refracted;
		float ReflectProb;
		float Cos;
		if (Dot(RayIn.direction(), Rec.Normal) > 0)
		{
			OutwardNormal = -Rec.Normal;
			NiOverNt = ReflectiveIndex;
			Cos = ReflectiveIndex * Dot(RayIn.direction(), Rec.Normal) / RayIn.direction().Length();
		}
		else
		{
			OutwardNormal = Rec.Normal;
			NiOverNt = 1.f / ReflectiveIndex;
			Cos = -Dot(RayIn.direction(), Rec.Normal) / RayIn.direction().Length();
		}

		if (Refract(RayIn.direction(), OutwardNormal, NiOverNt, Refracted))
		{
			Scattered = ray(Rec.Pos, Refracted);
			ReflectProb = Schlick(Cos, ReflectiveIndex);
		}
		else
		{
			Scattered = ray(Rec.Pos, Reflected);
			ReflectProb = 1.f;
		}

		if (RandomDouble() < ReflectProb)
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