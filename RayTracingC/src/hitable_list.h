#pragma once

#include "hitable.h"

class hitable_list : public hitable
{
public:
	hitable_list() {};
	hitable_list(hitable **L, int N) { List = L; ListSize = N; };
	virtual bool Hit(const ray& Ray, float TMin, float TMax, hit_record& Rec) const;

	hitable **List;
	int ListSize;
};

bool hitable_list::Hit(const ray& Ray, float TMin, float TMax, hit_record& Rec) const
{
	hit_record TempRec;
	bool HitAnything = false;
	double ClosestSoFar = TMax;
	for (int i = 0; i < ListSize; i++)
	{
		if (List[i]->Hit(Ray, TMin, ClosestSoFar, TempRec)) 
		{
			HitAnything = true;
			ClosestSoFar = TempRec.T;
			Rec = TempRec;
		}
	}
	return HitAnything;
}