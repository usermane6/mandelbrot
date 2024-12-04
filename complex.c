#include <SDL2/SDL.h>
#include "complex.h"

Cdub Cadd(Cdub u, Cdub v) {
    return (Cdub){ u.a + v.a, u.b + v.b};
}

Cdub Cmult(Cdub u, Cdub v) {
    return (Cdub){ u.a * v.a - u.b * v.b, u.b * v.a + u.a * v.b};
}

long double length_squared(Cdub u) {
    return u.a * u.a + u.b * u.b;
}

Uint64 mandelbrot(Cdub z, Uint64 num) {
    Uint64 count = 0;
    Cdub z1 = z;

    while (count <= num) {
        if (length_squared(z1) > 4.0) return count;

        z1 = Cadd(Cmult(z1,z1), z);
        count++;
    }
    return num;
}