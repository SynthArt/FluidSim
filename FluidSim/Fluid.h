#pragma once
#include <vector>
#include <raylib.h>

#define N 128
#define SCALE 6
#define g 16


class Fluid {
	float dt;
	float diff;
	float visc;
	float gridH;

	std::vector<std::vector<float>> density;	std::vector<std::vector<float>> s;
	std::vector<std::vector<float>> v;			std::vector<std::vector<float>> u;
	std::vector<std::vector<float>> v0;			std::vector<std::vector<float>> u0;

	std::vector<std::vector<float>> bf; // body forces - not working correctly
	
	public:
		Fluid(float dt, float diffusion, float viscosity);

	void fadeD();

	void addDensity(int x, int y, float dValue);

	void addVelocity(int x, int y, float amntX, float amntY);

	void renderDensity();

	void renderVelocity();

	void set_bnd(int b, std::vector<std::vector<float>>& x);
	
	void lin_solve(int B, std::vector<std::vector<float>>& x, const std::vector<std::vector<float>>& b, float a, float c);
	
	void add_source(std::vector<std::vector<float>>& x, const std::vector<std::vector<float>>& b);

	void diffuse(int b, std::vector<std::vector<float>> &dens, const std::vector<std::vector<float>> &dens0, float dFactor);

	void advect(int b, std::vector<std::vector<float>> &dens, const std::vector<std::vector<float>> &dens0, 
		const std::vector<std::vector<float>> &u, const std::vector<std::vector<float>> &v);

	void project(std::vector<std::vector<float>> &u, std::vector<std::vector<float>> &v, 
		std::vector<std::vector<float>> &div, std::vector<std::vector<float>> &p);

	void step();

	void forces_step();

	void dens_step();

	void advect_step();

	void SWAP(std::vector<std::vector<float>> x0, std::vector<std::vector<float>> x);

	int TI(int index);
};
