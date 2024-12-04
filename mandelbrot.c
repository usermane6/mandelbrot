#include "stdio.h"
#include <SDL2/SDL.h>

#include "complex.h"

#define SCREEN_WIDTH 801
#define SCREEN_HEIGHT SCREEN_WIDTH
#define PIXEL_DATA_LENGTH 4
#define ITERATIONS 100

typedef struct {
    long double xmax, xmin, ymax, ymin;
} View;

Cdub pixel_to_complex(size_t col, size_t row, View v) {
    Cdub complex;
    
    complex.a = v.xmin + (v.xmax - v.xmin) * ((long double) (col/4) / (long double) SCREEN_WIDTH);
    complex.b = v.ymin + (v.ymax - v.ymin) * ((long double) row / (long double) SCREEN_HEIGHT);

    return complex;
}

Cdub pixel_to_complex_f(long double col, long double row, View v) {
    Cdub complex;
    
    complex.a = v.xmin + (v.xmax - v.xmin) * ((col/4.) / (long double) SCREEN_WIDTH);
    complex.b = v.ymin + (v.ymax - v.ymin) * (row / (long double) SCREEN_HEIGHT);

    return complex;
}

double get_average_pixel(
    size_t col, 
    size_t row,
    double mult,
    View v
) {
    // needs to return on a scale of 0->255
    double total = 0.;
    unsigned int count = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            
            total += mandelbrot(pixel_to_complex_f((double)col + ((double)i * mult), (double)row + ((double)j * mult), v), ITERATIONS);

            count++;  
        }
    }
    
    return (total * 255) / (ITERATIONS * count);
}

void write_surface(SDL_Surface *surface, View v) {
    SDL_LockSurface(surface);

    Uint8 *pixels = (Uint8 *)surface->pixels;

    for (size_t row = 0; row < surface->h; row++) {
        for (size_t col = 0; col < surface->pitch; col += PIXEL_DATA_LENGTH) {
            
            double c = get_average_pixel(col, row, 100, v);

            Uint64 base_id = surface->pitch * row + col;
            //BGRA8888 pixel format
            pixels[base_id + 0] = 0x00; // Blue
            pixels[base_id + 1] = 0x00; // Green
            pixels[base_id + 2] = (Uint8)c; // Red

            // if (c == 255) {
            //     pixels[base_id + 0] = 0x00; // Blue
            //     pixels[base_id + 1] = 0x00; // Green
            //     pixels[base_id + 2] = 0xff; // Red
            // } else {
            //     pixels[base_id + 0] = 0; // Blue
            //     pixels[base_id + 1] = 0; // Green
            //     pixels[base_id + 2] = 0; // Red
            //     // pixels[base_id + 0] = 0xff; // Blue
            //     // pixels[base_id + 1] = 0xff; // Green
            //     // pixels[base_id + 2] = 0xff; // Red
            // }
        }
    }
    
    printf("done!\n");

    SDL_UnlockSurface(surface); 
}

View click_box(View v, long double r) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    size_t col = (size_t)x, row = (size_t)y;
    Cdub coords = pixel_to_complex(col * 4, row, v);
    
    // printf("(%f, %f), (%d, %d), %f \n", coords.a, coords.b, x, y, r);

    return (View){coords.a + r, coords.a - r, coords.b + r, coords.b - r};
}

int main(int argc, char *args[]) {
    SDL_Window *window = NULL;
    SDL_Surface *screen_surface = NULL;

    // printf("%zu", mandelbrot((Cdub){0.25, 0.75}, 6));


    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); 
    if ( window == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    screen_surface = SDL_GetWindowSurface(window);
    
    long double r = 2;
    View v = (View){r, -r, r, -r};

    write_surface(screen_surface, v);
    SDL_UpdateWindowSurface( window );

    SDL_Event e;
    SDL_bool running = SDL_TRUE;
    SDL_bool accept_input = SDL_TRUE;

    while (running) {
        while ( SDL_PollEvent( &e ) ) { 
            if ( e.type == SDL_QUIT ) running = SDL_FALSE; 
            if ( e.type == SDL_MOUSEBUTTONDOWN ) {
                SDL_MouseButtonEvent *b = &e;
                if (b->button == SDL_BUTTON_LEFT) {

                    r = r / 2;
                    v = click_box(v, r);
                    // click_box(v, r);

                    write_surface(screen_surface, v);
                    SDL_UpdateWindowSurface( window );
                };
                
            }
        }
    }

    SDL_DestroyWindow( window );
    SDL_Quit();
    return 0;
}

