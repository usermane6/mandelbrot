# ifndef COMPLEX
# define COMPLEX 1 

#include <SDL2/SDL.h>

typedef struct {
    long double a, b;
} Cdub;

Cdub Cadd(Cdub u, Cdub v);

Cdub Cmult(Cdub u, Cdub v);

long double length_squared(Cdub u);

Uint64 mandelbrot(Cdub z, Uint64 num);

# endif