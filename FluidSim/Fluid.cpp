#include "Fluid.h"
//#include "raylib.h"
#include <algorithm> 

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

Fluid::Fluid(float dt, float diffusion, float viscosity) {
    this->dt = dt;
    this->diff = diffusion;
    this->visc = viscosity;
    this->gridH = SCALE;
    //this.grvity = new float[N][N];

    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            density.resize(N);  s.resize(N);
            v.resize(N);        u.resize(N);
            v0.resize(N);       u0.resize(N);
            bf.resize(N);
        }
    }
    
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
            //Color densityClr = { 0, densityValue, densityValue};
            //DrawRectangle(x,y,SCALE,SCALE,densityClr);
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
                //DrawLine(x, y, x + vx * SCALE, y + vy * SCALE, {255,255,255} );
            }
        }
    }
}

void Fluid::add_source(std::vector<std::vector<float>> x, std::vector<std::vector<float>> b) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            x[TI(i)][TI(j)] += dt * b[TI(i)][TI(j)];
    }
}

void Fluid::set_bnd(int b, std::vector<std::vector<float>> x) {
    for (int i = 1; i < N - 1; i++) {

        x[0][TI(i)] = (b == 1 ? -x[1][TI(i)] : x[1][TI(i)]);

        x[TI(N - 1)][TI(i)] = (b == 1 ? -x[TI(N - 2)][TI(i)] : x[TI(N - 2)][TI(i)]);

        x[TI(i)][0] = (b == 2 ? -x[TI(i)][1] : x[TI(i)][1]);

        x[TI(i)][TI(N - 1)] = (b == 2 ? -x[TI(i)][TI(N - 2)] : x[TI(i)][TI(N - 2)]);
    }
    x[0][0] = 0.5f * (x[1][0] + x[0][1]);
    x[0][TI(N - 1)] = 0.5f * (x[1][TI(N - 1)] + x[0][TI(N - 2)]);
    x[TI(N - 1)][0] = 0.5f * (x[TI(N - 2)][0] + x[TI(N - 1)][1]);
    x[TI(N - 1)][TI(N - 1)] = 0.5f * (x[TI(N - 2)][TI(N - 1)] + x[TI(N - 1)][TI(N - 2)]);
}

void Fluid::lin_solve(int B, std::vector<std::vector<float>> x, std::vector<std::vector<float>> b, float a, float c) {
    int i, j, iter;
    // x is the initial density
    // b is the previous density step
    for (iter = 0; iter < g; iter++) {
        for (i = 1; i < N - 1; i++) {
            for (j = 1; j < N - 1; j++) {
                x[TI(i)][TI(j)] = (b[TI(i)][TI(j)] + a * (x[TI(i + 1)][TI(j)] + x[TI(i - 1)][TI(j)] + x[TI(i)][TI(j + 1)] + x[TI(i)][TI(j - 1)])) / c;
                // pseudo code below v
                //denseNext[i,j] = ( densityInitial[i,j] + k * (densityInitial[i+1,j] + densityInitial[i-1,j] + densityInitial[i,j+1] + densityInitial[i,j-1]) ) / (1 + k);
            }
        }
        set_bnd(B, x);
    }
}

void Fluid::diffuse(int b, std::vector<std::vector<float>> dens, std::vector<std::vector<float>> dens0, float dFactor) {
    float k = dt * dFactor * pow(SCALE, -2);
    lin_solve(b, dens, dens0, k, 1 + 4 * k);
}

void Fluid::advect(int b, std::vector<std::vector<float>> dens, std::vector<std::vector<float>> dens0, std::vector<std::vector<float>> u, std::vector<std::vector<float>> v) {
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
    dt0 = dt * pow(SCALE, 2); // advection rate
    for (i = 1; i < N - 1; i++) {
        for (j = 1; j < N - 1; j++) { // flag: making this to N-2 diffuses velocity field at normal rate but causes boundary issues
            // backtrace the position to the previous cell
            x = i - dt0 * u[TI(i)][TI(j)];
            y = j - dt0 * v[TI(i)][TI(j)];
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

            dens[TI(i)][TI(j)] = s0 * (t0 * dens0[TI(i0)][TI(j0)] + t1 * dens0[TI(i0)][TI(j1)])
                + s1 * (t0 * dens0[TI(i1)][TI(j0)] + t1 * dens0[TI(i1)][TI(j1)]);
        }
    }
    set_bnd(b, dens);
}

void Fluid::project(std::vector<std::vector<float>> u, std::vector<std::vector<float>> v, std::vector<std::vector<float>> div, std::vector<std::vector<float>> p) {
    // compute divergence
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            // poisson equation; also note that div would ideally equal 0
            div[TI(i)][TI(j)] = -0.5 * (u[TI(i + 1)][TI(j)] - u[TI(i - 1)][TI(j)]
                + v[TI(i)][TI(j + 1)] - v[TI(i)][TI(j - 1)]) / SCALE;
            p[TI(i)][TI(j)] = 0.0;
        }
    }
    set_bnd(0, div); set_bnd(0, p);
    // solve poisson equation
    lin_solve(0, p, div, 1, 6);
    // subtract gradient field
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            u[TI(i)][TI(j)] -= 0.5 * SCALE * (p[TI(i + 1)][TI(j)] - p[TI(i - 1)][TI(j)]);
            v[TI(i)][TI(j)] -= 0.5 * SCALE * (p[TI(i)][TI(j + 1)] - p[TI(i - 1)][TI(j - 1)]);
        }
    }
    set_bnd(1, u); set_bnd(2, v);
}

void Fluid::step() {
    // u is vx and v is vy
    advect_step();
    dens_step();
    forces_step();
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
    //project(u0, v0, u, v);
    //project(u, v, u0, v0);
    //pow(9.18,-2) * density[i][j];
    //add_source(v0,bf);
}

void Fluid::dens_step() {
    /*
     add_source(density, s);
     SWAP(density,s);
     diffuse(0, density,s);

     SWAP(density,s);
     advect(0,density,s,u,v);
     */
    add_source(s, density);
    //SWAP(density,s);
    diffuse(0, s, density, diff);
    //SWAP(density,s);
    advect(0, density, s, u, v);
}

void Fluid::advect_step() {
    /*
     add_source(u, u0); add_source(v, v0);
     SWAP(u,u0); SWAP(v,v0);
     diffuse(1,u,u0);
     diffuse(2,v,v0);
     SWAP(u,u0); SWAP(v,v0);
     advect(1,u,u0,u0,v0);
     advect(2,v,v0,u0,v0);

     project(u,v,u0,v0);
     */
    add_source(u, u0); add_source(v, v0);
    diffuse(1, u0, u, visc);
    diffuse(2, v0, v, visc);

    project(u0, v0, u, v);

    advect(1, u, u0, u0, v0);
    advect(2, v, v0, u0, v0);

    project(u, v, u0, v0);
}

void Fluid::SWAP(std::vector<std::vector<float>> x0, std::vector<std::vector<float>> x) {
    x0.swap(x);
    /*
    float temp[N][N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            temp[i][j] = *x0[i][j];
            *x0[i][j] = *x[i][j];
            *x[i][j] = temp[i][j];
        }
    }
    */
}

int Fluid::TI(int index)
{
    return index;
}
