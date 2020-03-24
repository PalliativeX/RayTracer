#include <iostream>
#include <fstream>

#include "sphere.h"
#include "hitable_list.h"
#include "ray.h"
#include "camera.h"

vec3 color(const ray& Ray, hitable *World)
{
	hit_record Rec;
	if (World->hit(Ray, 0.f, FLT_MAX, Rec))
	{
		return 0.5f * vec3(Rec.Normal.x()+1.f, Rec.Normal.y()+1.f, Rec.Normal.z()+1.f);
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
	int Width = 400;
	int Height = 200;
	int Ns = 200;

	std::ofstream File;
	File.open("image.ppm");
	File << "P3\n" << Width << " " << Height << "\n255\n";

	hitable *List[2];
	List[0] = new sphere(vec3(0, 0, -1.f), 0.5f);
	List[1] = new sphere(vec3(0, -100.5f, -1.f), 100.f);
	hitable *World = new hitable_list(List, 2);

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
				Color += color(Ray, World);
			}
			Color /= float(Ns);

			int RedInt   = int(255.99f * Color[0]);
			int GreenInt = int(255.99f * Color[1]);
			int BlueInt  = int(255.99f * Color[2]);

			File << RedInt << " " << GreenInt << " " << BlueInt << "\n";
		}
	}

	File.close();
	return 0;
}