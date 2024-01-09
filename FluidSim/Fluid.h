#pragma once
#include <vector>
#include <raylib.h>

constexpr auto N = 128; // 128
constexpr auto SCALE = 6; // 6
constexpr auto G = 12; // 12

class Fluid {
private:
	float dt;
	float diff;
	float visc;
	float gridH;
	float density[N][N]; float s[N][N];
	float v[N][N]; float u[N][N];
	float v0[N][N];	float u0[N][N];

	bool walls[N][N];
	//float bf[N][N]; // body forces - not working correctly 

	void applyBounds(float (&x)[N][N]) const;
	void lin_solve(int B, float (&x)[N][N], const float (&b)[N][N], float a, float c);
	void add_source(float (&x)[N][N], const float (&b)[N][N]) const;
	void diffuse(int b, float (&dens)[N][N], const float (&dens0)[N][N], float dFactor);
	void advect(int b, float (&dens)[N][N], const float (&dens0)[N][N], const float (&u)[N][N], const float (&v)[N][N]);
	void project(float (&u)[N][N], float (&v)[N][N], float (&div)[N][N], float (&p)[N][N]);

	void forces_step();
	void dens_step();
	void advect_step();

public:
	Fluid(float dt, float diffusion, float viscosity);
	void fadeD();
	void addWall(int x, int y);
	void addDensity(int x, int y, float dValue);
	void addVelocity(int x, int y, float amntX, float amntY);
	void render() const; // renders Density and Walls
	void renderVelocity() const;
	void step();
	void updateDeltaTime(float dt);
};
