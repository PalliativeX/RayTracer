#pragma once

#include <cstdlib>

inline double RandomDouble() {
	return rand() / (RAND_MAX + 1.f);
}