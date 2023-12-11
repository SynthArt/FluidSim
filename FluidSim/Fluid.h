#pragma once
#include <vector>
class Fluid {
	float dt;
	float diff;
	float visc;
	float gridH;

	std::vector<float> density, s;
	std::vector<float> v, u;
	std::vector<float> v0, u0;

	std::vector<float> bf; // body forces - not working correctly

	Fluid(float dt, float diffusion, float viscocity) {
	
	}
};
