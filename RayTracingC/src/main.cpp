#include <iostream>
#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "material.h"
#include "sphere.h"
#include "hitable_list.h"
#include "ray.h"
#include "camera.h"

vec3 Color(const ray& Ray, hitable *World, int Depth)
{
	hit_record Rec;
	if (World->Hit(Ray, 0.001f, FLT_MAX, Rec))
	{
		ray Scattered;
		vec3 Attenuation;
		if (Depth < 50 && Rec.MatPtr->Scatter(Ray, Rec, Attenuation, Scattered))
		{
			return Attenuation * Color(Scattered, World, Depth + 1);
		}
		else
		{
			return vec3(0, 0, 0);
		}
	}
	else
	{
		vec3 UnitDirection = UnitVector(Ray.direction());
		float T = 0.5f * (UnitDirection.y() + 1.f);
		return (1.f - T) * vec3(1.f, 1.f, 1.f) + T * vec3(0.5f, 0.7f, 1.f);
	}
}

hitable *RandomScene() 
{
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++) 
		{
			float choose_mat = RandomDouble();
			vec3 center(a + 0.9*RandomDouble(), 0.2, b + 0.9*RandomDouble());
			if ((center - vec3(4, 0.2, 0)).Length() > 0.9)
			{
				if (choose_mat < 0.8) 
				{  // diffuse
					list[i++] = new sphere(
						center, 0.2,
						new lambertian(vec3(RandomDouble()*RandomDouble(),
									        RandomDouble()*RandomDouble(),
											RandomDouble()*RandomDouble()))
					);
				}
				else if (choose_mat < 0.95) 
				{ // metal
					list[i++] = new sphere(
						center, 0.2,
						new metal(vec3(0.5*(1 + RandomDouble()),
							0.5*(1 + RandomDouble()),
							0.5*(1 + RandomDouble())),
							0.5*RandomDouble())
					);
				}
				else 
				{  // glass
					list[i++] = new sphere(center, 0.2f, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0.f, 1.f, 0.f), 1.f, new dielectric(1.5f));
	list[i++] = new sphere(vec3(-4.f, 1.f, 0.f), 1.f, new lambertian(vec3(0.4f, 0.2f, 0.1f)));
	list[i++] = new sphere(vec3(4.f, 1.f, 0.f), 1.f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.f));

	return new hitable_list(list, i);
}

int main()
{
	int Width = 800;
	int Height = 400;
	int Ns = 10;
	int ChannelNum = 3;

	uint8_t *Pixels = new uint8_t[Width * Height * ChannelNum];

	// NOTE: Conventional output to a .ppm file
	//std::ofstream File;
	//File.open("image.ppm");
	//File << "P3\n" << Width << " " << Height << "\n255\n";

	hitable *World = RandomScene();

	vec3 LookFrom(13.f, 2.f, 3.f);
	vec3 LookAt(0.f, 0.f, 0.f);
	float DistToFocus = 10.f;
	float Aperture = 0.1f;

	camera Cam(LookFrom, LookAt, vec3(0, 1, 0), 20, float(Width)/float(Height), Aperture, DistToFocus);

	int Index = 0;
	// From top to bottom
	for (int j = Height - 1; j >= 0; j--)
	{
		for (int i = 0; i < Width; i++)
		{
			vec3 Col(0, 0, 0);
			for (int s = 0; s < Ns; s++)
			{
				float U = float(i + RandomDouble()) / float(Width);
				float V = float(j + RandomDouble()) / float(Height);
				ray Ray = Cam.GetRay(U, V);

				vec3 P = Ray.point_at_parameter(2.f);
				Col += Color(Ray, World, 0);
			}
			Col /= float(Ns);
			Col = vec3(sqrt(Col[0]), sqrt(Col[1]), sqrt(Col[2]));

			int RedInt   = int(255.99f * Col[0]);
			int GreenInt = int(255.99f * Col[1]);
			int BlueInt  = int(255.99f * Col[2]);

			Pixels[Index++] = RedInt;
			Pixels[Index++] = GreenInt;
			Pixels[Index++] = BlueInt;

			//File << RedInt << " " << GreenInt << " " << BlueInt << "\n";
		}

		float Readiness = (1.f - ((float)j / (float)Height)) * 100.f;
		printf("Readiness: %.2f%% \n", Readiness);
	}

	stbi_write_png("image.png", Width, Height, ChannelNum, Pixels, Width * ChannelNum);

	//File.close();
	return 0;
}