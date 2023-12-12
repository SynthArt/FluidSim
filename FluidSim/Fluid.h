#pragma once
#include <vector>

#define N 128
#define SCALE 6
#define g 16


class Fluid {
	float dt;
	float diff;
	float visc;
	float gridH;

	float density[N][N];	float s[N][N];
	float v[N][N];			float u[N][N];
	float v0[N][N];			float u0[N][N];

	float bf[N][N]; // body forces - not working correctly

	Fluid(float dt, float diffusion, float viscosity) {
		this->dt = dt;
		this->diff = diffusion;
		this->visc = viscosity;
		this->gridH = SCALE;
		//this.grvity = new float[N][N];
	}

	void fadeD();

	void addDensity(int x, int y, float dValue);

	void addVelocity(int x, int y, float amntX, float amntY);

	void renderDensity();

	void renderVelocity();

	void add_source(float (*x)[N][N], float (*b)[N][N]);

	void set_bnd(int b, float (*x)[N][N]);

	void lin_solve(int B, float (*x)[N][N], float (*b)[N][N], float a, float c);

	void diffuse(int b, float (*dens)[N][N], float (*dens0)[N][N], float dFactor);

	void advect(int b, float (*dens)[N][N], float (*dens0)[N][N], float (*u)[N][N], float (*v)[N][N]);

	void project(float (*u)[N][N], float (*v)[N][N], float (*div)[N][N], float (*p)[N][N]);

	void step();

	void forces_step();

	void dens_step();

	void advect_step();

	void SWAP(float x0, float (*x)[N][N]);

	int TI(int index);
};
