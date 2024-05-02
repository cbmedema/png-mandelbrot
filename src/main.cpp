#include "mandelbrot.h"

int main(int argc, char* argv[])
{
    Mandelbrot fractal(-0.770065,0.110021,125000,5000000);
    fractal.gen_fractal();
    fractal.gen_png();
}
