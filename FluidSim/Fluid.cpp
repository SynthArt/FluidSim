#include "Fluid.h"
#include <raylib.h>

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

void Fluid::fadeD() {
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            // fade the grid so we can keep drawing but be considerate of delta time or the spatial time step
            this->density[x][y] = constrain(this->density[x][y] - 1 * 0.125f, 0.f, 255.f);
}

void Fluid::addDensity(int x, int y, float dValue) {
    if (x >= N || y >= N || x < 0 || y < 0)
        return;
    this->density[TI(x)][TI(y)] += dValue;
}

void Fluid::addVelocity(int x, int y, float amntX, float amntY) {
    if (x >= N || y >= N || x < 0 || y < 0)
        return;
    this->u[x][y] += amntX;
    this->v[x][y] += amntY;
}

void Fluid::renderDensity() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float x = i * SCALE;
            float y = j * SCALE;
            float densityValue = this->density[TI(i)][TI(j)];
            //colorMode(HSB,360,100,100);
            Color densityClr = ColorFromNormalized({ 0, densityValue, densityValue, 0 });
            DrawRectangle(x,y,SCALE,SCALE,densityClr);
        }
    }
}

void Fluid::renderVelocity() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float x = i * SCALE;
            float y = j * SCALE;
            float vx = this->u[TI(i)][TI(j)];
            float vy = this->v[TI(i)][TI(j)];
            if (!(abs(vx) < 0.1 && abs(vy) <= 0.1)) {
                DrawLine(x, y, x+vx * SCALE, y+vy * SCALE, Color(WHITE));
            }
        }
    }
}

void Fluid::add_source(float(*x)[N][N], float(*b)[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            *x[TI(i)][TI(j)] += dt * *b[TI(i)][TI(j)];
    }
}

void Fluid::set_bnd(int b, float(*x)[N][N]) {
    for (int i = 1; i < N - 1; i++) {

        *x[0][TI(i)] = (b == 1 ? -*x[1][TI(i)] : *x[1][TI(i)]);

        *x[TI(N - 1)][TI(i)] = (b == 1 ? -*x[TI(N - 2)][TI(i)] : *x[TI(N - 2)][TI(i)]);

        *x[TI(i)][0] = (b == 2 ? -*x[TI(i)][1] : *x[TI(i)][1]);

        *x[TI(i)][TI(N - 1)] = (b == 2 ? -*x[TI(i)][TI(N - 2)] : *x[TI(i)][TI(N - 2)]);
    }
    *x[0][0] = 0.5f * (*x[1][0] + *x[0][1]);
    *x[0][TI(N - 1)] = 0.5f * (*x[1][TI(N - 1)] + *x[0][TI(N - 2)]);
    *x[TI(N - 1)][0] = 0.5f * (*x[TI(N - 2)][0] + *x[TI(N - 1)][1]);
    *x[TI(N - 1)][TI(N - 1)] = 0.5f * (*x[TI(N - 2)][TI(N - 1)] + *x[TI(N - 1)][TI(N - 2)]);
}

void Fluid::lin_solve(int B, float(*x)[N][N], float(*b)[N][N], float a, float c) {
    int i, j, iter;
    // x is the initial density
    // b is the previous density step
    for (iter = 0; iter < g; iter++) {
        for (i = 1; i < N - 1; i++) {
            for (j = 1; j < N - 1; j++) {
                *x[TI(i)][TI(j)] = (*b[TI(i)][TI(j)] + a * (*x[TI(i + 1)][TI(j)] + *x[TI(i - 1)][TI(j)] + *x[TI(i)][TI(j + 1)] + *x[TI(i)][TI(j - 1)])) / c;
                // pseudo code below v
                //denseNext[i,j] = ( densityInitial[i,j] + k * (densityInitial[i+1,j] + densityInitial[i-1,j] + densityInitial[i,j+1] + densityInitial[i,j-1]) ) / (1 + k);
            }
        }
        set_bnd(B, x);
    }
}

void Fluid::diffuse(int b, float(*dens)[N][N], float(*dens0)[N][N], float dFactor) {
    float k = dt * dFactor * pow(SCALE, -2);
    lin_solve(b, dens, dens0, k, 1 + 4 * k);
}

void Fluid::advect(int b, float(*dens)[N][N], float(*dens0)[N][N], float(*u)[N][N], float(*v)[N][N]) {
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
    dt0 = dt * pow(SCALE, 2); // advection rate
    for (i = 1; i < N - 1; i++) {
        for (j = 1; j < N - 1; j++) { // flag: making this to N-2 diffuses velocity field at normal rate but causes boundary issues
            // backtrace the position to the previous cell
            x = i - dt0 * *u[TI(i)][TI(j)];
            y = j - dt0 * *v[TI(i)][TI(j)];
            // clamp the position to the center of each cell
            if (x < 0.5) x = 0.5; if (x > (N - 2) + 0.5) x = (N - 2) + 0.5;
            if (y < 0.5) y = 0.5; if (y > (N - 2) + 0.5) y = (N - 2) + 0.5;
            // current cell's center
            i0 = (int)x; j0 = (int)y;
            // cell's neighbor
            i1 = i0 + 1; j1 = j0 + 1;
            // offsets from center
            s1 = x - i0; s0 = 1 - s1;
            t1 = y - j0; t0 = 1 - t1;

            *dens[TI(i)][TI(j)] = s0 * (t0 * *dens0[TI(i0)][TI(j0)] + t1 * *dens0[TI(i0)][TI(j1)])
                + s1 * (t0 * *dens0[TI(i1)][TI(j0)] + t1 * *dens0[TI(i1)][TI(j1)]);
        }
    }
    set_bnd(b, dens);
}

void Fluid::project(float(*u)[N][N], float(*v)[N][N], float(*div)[N][N], float(*p)[N][N]) {
    // compute divergence
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            // poisson equation; also note that div would ideally equal 0
            *div[TI(i)][TI(j)] = -0.5 * (*u[TI(i + 1)][TI(j)] - *u[TI(i - 1)][TI(j)]
                + *v[TI(i)][TI(j + 1)] - *v[TI(i)][TI(j - 1)]) / SCALE;
            *p[TI(i)][TI(j)] = 0.0;
        }
    }
    set_bnd(0, div); set_bnd(0, p);
    // solve poisson equation
    lin_solve(0, p, div, 1, 6);
    // subtract gradient field
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            *u[TI(i)][TI(j)] -= 0.5 * SCALE * (*p[TI(i + 1)][TI(j)] - *p[TI(i - 1)][TI(j)]);
            *v[TI(i)][TI(j)] -= 0.5 * SCALE * (*p[TI(i)][TI(j + 1)] - *p[TI(i - 1)][TI(j - 1)]);
        }
    }
    set_bnd(1, u); set_bnd(2, v);
}
