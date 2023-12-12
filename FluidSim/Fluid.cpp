#include "Fluid.h"

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
