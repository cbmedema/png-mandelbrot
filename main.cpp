#include "mandelbrot.h"

int main(int argc, char* argv[])
{
    Mandelbrot fractal(-0.101096,0.956286,128, 500);
    fractal.gen_fractal();
    fractal.gen_png();
}
