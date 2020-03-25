#include <iostream>
#include <fstream>

#include "material.h"
#include "sphere.h"
#include "hitable_list.h"
#include "ray.h"
#include "camera.h"

vec3 color(const ray& Ray, hitable *World, int Depth)
{
	hit_record Rec;
	if (World->hit(Ray, 0.001f, FLT_MAX, Rec))
	{
		ray Scattered;
		vec3 Attenuation;
		if (Depth < 50 && Rec.MatPtr->scatter(Ray, Rec, Attenuation, Scattered))
		{
			return Attenuation * color(Scattered, World, Depth + 1);
		}
		else
		{
			return vec3(0, 0, 0);
		}
	}
	else
	{
		vec3 UnitDirection = unit_vector(Ray.direction());
		float T = 0.5f * (UnitDirection.y() + 1.f);
		return (1.f - T) * vec3(1.f, 1.f, 1.f) + T * vec3(0.5f, 0.7f, 1.f);
	}
}

int main()
{
	int Width = 200;
	int Height = 100;
	int Ns = 100;

	std::ofstream File;
	File.open("image.ppm");
	File << "P3\n" << Width << " " << Height << "\n255\n";

	hitable *List[5];
	List[0] = new sphere(vec3(0, 0, -1.f), 0.5f, new lambertian(vec3(0.8f, 0.3f, 0.3f)));
	List[1] = new sphere(vec3(0, -100.5f, -1.f), 100.f, new lambertian(vec3(0.8f, 0.8f, 0.0f)));
	List[2] = new sphere(vec3(1.f, 0.f, -1.f), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f), 0.3f));
	List[3] = new sphere(vec3(-1.f, 0.f, -1.f), 0.5f, new dielectric(1.5f));
	List[4] = new sphere(vec3(-1.f, 0.f, -1.f), -0.45f, new dielectric(1.5f));
	hitable *World = new hitable_list(List, 5);

	camera Cam;

	// From top to bottom
	for (int j = Height - 1; j >= 0; j--)
	{
		for (int i = 0; i < Width; i++)
		{
			vec3 Color(0, 0, 0);
			for (int s = 0; s < Ns; s++)
			{
				float U = float(i) / float(Width);
				float V = float(j) / float(Height);
				ray Ray = Cam.getRay(U, V);

				vec3 P = Ray.point_at_parameter(2.f);
				Color += color(Ray, World, 0);
			}
			Color /= float(Ns);
			Color = vec3(sqrt(Color[0]), sqrt(Color[1]), sqrt(Color[2]));

			int RedInt   = int(255.99f * Color[0]);
			int GreenInt = int(255.99f * Color[1]);
			int BlueInt  = int(255.99f * Color[2]);

			File << RedInt << " " << GreenInt << " " << BlueInt << "\n";
		}
	}

	File.close();
	return 0;
}