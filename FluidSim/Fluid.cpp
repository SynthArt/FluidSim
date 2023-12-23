#include "Fluid.h"
#include <algorithm> 
#include <iostream>

const int N = 128;
const int SCALE = 6;
const int G = 8;

template<class T>
const T& constrain(const T& x, const T& a, const T& b) {
    if (x < a) {
        return a;
    }
    else if (b < x) {
        return b;
    }
    else
        return x;
}

int map(float input, float inputStart, float inputEnd, int outputStart, int outputEnd ) {
    float slopeDenom = (inputEnd - inputStart);
    float slope = 0;
    if (slopeDenom != 0)
        slope = (outputEnd - outputStart) / slopeDenom;
    int output = outputStart + slope * (input - inputStart);
    return output;
}

Fluid::Fluid(float dt, float diffusion, float viscosity) {
    this->dt = dt;
    this->diff = diffusion;
    this->visc = viscosity;
    this->gridH = SCALE;
    //this.grvity = new float[N][N];
    density.resize(N, std::vector<float>(N));  s.resize(N, std::vector<float>(N));
    v.resize(N, std::vector<float>(N));        u.resize(N, std::vector<float>(N));
    v0.resize(N, std::vector<float>(N));       u0.resize(N, std::vector<float>(N));
    bf.resize(N, std::vector<float>(N));
}

void Fluid::fadeD() {
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            // fade the grid so we can keep drawing but be considerate of delta time or the spatial time step
            this->density[x][y] = constrain(this->density[x][y] - 1 * 0.125f, 0.f, 255.f);
}

void Fluid::addDensity(int x, int y, float dValue) {
    if (x >= N || y >= N || x < 0 || y < 0)
        return;
    this->density[x][y] += dValue;
}

void Fluid::addVelocity(int x, int y, float amntX, float amntY) {
    if (x >= N || y >= N || x < 0 || y < 0)
        return;
    this->u[x][y] += amntX;
    this->v[x][y] += amntY;
}

void Fluid::renderDensity() {
    for (int i = 0; i < N; i++) {
        float x = i * SCALE;
        for (int j = 0; j < N; j++) {
            float y = j * SCALE;
            
            float densityValue = this->density[i][j];
            if (densityValue < 0)
                densityValue = 0;
            if (densityValue > 255)
                densityValue = 255;
           
            Color densityClr = { 0, densityValue, densityValue, 255 };
            DrawRectangle(x,y,SCALE,SCALE,densityClr);
        }
    }
}

void Fluid::renderVelocity() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float x = i * SCALE;
            float y = j * SCALE;
            float vx = this->u[i][j];
            float vy = this->v[i][j];
            if (!(abs(vx) < 0.1f && abs(vy) <= 0.1f)) {
                DrawLine(x, y, x + vx * SCALE, y + vy * SCALE, {255,255,255,255} );
            }
        }
    }
}

void Fluid::add_source(std::vector<std::vector<float>> &x, const std::vector<std::vector<float>> &b) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            x[i][j] += dt * b[i][j];
    }
}

void Fluid::set_bnd(int b, std::vector<std::vector<float>> &x) {
    for (int i = 1; i < N - 1; i++) {

        x[0][i] = (b == 1 ? -x[1][i] : x[1][i]);

        x[N - 1][i] = (b == 1 ? -x[N - 2][i] : x[N - 2][i]);

        x[i][0] = (b == 2 ? -x[i][1] : x[i][1]);

        x[i][N - 1] = (b == 2 ? -x[i][N - 2] : x[i][N - 2]);
    }
    x[0][0] = 0.5f * (x[1][0] + x[0][1]);
    x[0][N - 1] = 0.5f * (x[1][N - 1] + x[0][N - 2]);
    x[N - 1][0] = 0.5f * (x[N - 2][0] + x[N - 1][1]);
    x[N - 1][N - 1] = 0.5f * (x[N - 2][N - 1] + x[N - 1][N - 2]);
}

void Fluid::lin_solve(int B, std::vector<std::vector<float>> &x, const std::vector<std::vector<float>> &b, float a, float c) {
    int i, j, iter;
    // x is the initial density
    // b is the previous density step
    const float inverseC = 1.f / c;
    const float aTimesC = a * inverseC;
    for (iter = 0; iter < G; iter++) {
        for (i = 1; i < N - 1; i++) {
            for (j = 1; j < N - 1; j++) {
                float neighborSum = x[i + 1][j] + x[i - 1][j] + x[i][j + 1] + x[i][j - 1];
                x[i][j] = (b[i][j] + aTimesC * neighborSum) * inverseC;
            }
        }
        set_bnd(B, x);
    }
}

void Fluid::diffuse(int b, std::vector<std::vector<float>> &dens, const std::vector<std::vector<float>> &dens0, float dFactor) {
    float k = dt * dFactor * pow(SCALE, -2);
    lin_solve(b, dens, dens0, k, 1.0f + 4.0f * k);
}

void Fluid::advect(int b, std::vector<std::vector<float>> &dens, const std::vector<std::vector<float>> &dens0, const std::vector<std::vector<float>> &u, const std::vector<std::vector<float>> &v) {
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1;
    const float dt0 = dt * (SCALE * SCALE); // advection rate
    const float NMinus2Half = (N - 2) + 0.5f;


    for (i = 1; i < N - 1; i++) {

        for (j = 1; j < N - 1; j++) { // flag: making this to N-2 diffuses velocity field at normal rate but causes boundary issues
            // backtrace the position to the previous cell
            x = i - dt0 * u[i][j];
            y = j - dt0 * v[i][j];
            // clamp the position to the center of each cell
            if (x < 0.5f) x = 0.5f; if (x > NMinus2Half) x = NMinus2Half;
            if (y < 0.5f) y = 0.5f; if (y > NMinus2Half) y = NMinus2Half;
            // current cell's center
            i0 = floor(x); j0 = floor(y);
            // cell's neighbor
            i1 = i0 + 1; j1 = j0 + 1;
            // offsets from center
            s1 = x - i0; s0 = 1.f - s1;
            t1 = y - j0; t0 = 1.f - t1;

            dens[i][j] = s0 * (t0 * dens0[i0][j0] + t1 * dens0[i0][j1])
                + s1 * (t0 * dens0[i1][j0] + t1 * dens0[i1][j1]);

        }
    }
    set_bnd(b, dens);
}

void Fluid::project(std::vector<std::vector<float>> &u, std::vector<std::vector<float>> &v, std::vector<std::vector<float>> &div, std::vector<std::vector<float>> &p) {
    const float scaleCoeff = 0.5f * SCALE;
    // compute divergence
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            // poisson equation; also note that div would ideally equal 0
            float du = u[i + 1][j] - u[i - 1][j];
            float dv = v[i][j + 1] - v[i][j - 1];

            div[i][j] = -0.5f * (du + dv) / SCALE;
            p[i][j] = 0.f;
        }
    }
    set_bnd(0, div); set_bnd(0, p);
    // solve poisson equation
    lin_solve(0, p, div, 1, 6);
    // subtract gradient field
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            u[i][j] -= scaleCoeff * (p[i + 1][j] - p[i - 1][j]);
            v[i][j] -= scaleCoeff * (p[i][j + 1] - p[i - 1][j - 1]);
        }
    }
    set_bnd(1, u); set_bnd(2, v);
}

void Fluid::step() {
    // u is vx and v is vy
    advect_step(); // CAUSING MAJOR LAG
    dens_step();
    //forces_step();
}

void Fluid::forces_step() {
    /* gravity body force */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            v0[i][j] += 9.81f / (SCALE * SCALE);
            u0[i][j] *= 0.0f;
            //bf[i][j] = 9.81/(SCALE*1);
        }
    }
}

void Fluid::dens_step() {
    add_source(s, density);
    diffuse(0, s, density, diff);
    advect(0, density, s, u, v);
}

void Fluid::advect_step() {
    add_source(u, u0); add_source(v, v0);
    diffuse(1, u0, u, visc);
    diffuse(2, v0, v, visc);

    project(u0, v0, u, v);

    advect(1, u, u0, u0, v0);
    advect(2, v, v0, u0, v0);

    project(u, v, u0, v0);
}


