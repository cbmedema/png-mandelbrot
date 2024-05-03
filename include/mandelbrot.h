

#ifndef MANDELBROT_H
#define MANDELBROT_H

#endif //MANDELBROT_H

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "lodepng.h"
#include <thread>
#include <chrono>
#include "read_csv.h"

class Mandelbrot
{
private:
    const std::string color_map_file = "../color_maps/21bit.csv";
    const std::string output_file = "output.png";
    ReadCSV rcsv = ReadCSV(color_map_file);
    // device resolution variables
    const int resolution_x = 3840*1;
    const int resolution_y = 2160*1;
    const double aspect_ratio = resolution_x / static_cast<double>(resolution_y);
    std::vector<unsigned char> image;

    // fractal variables
    double center_real, center_imag, zoom_factor;
    int max_iterations;
    double start_real, start_imag;

    std::vector<unsigned char> calc_rgb_modulo(double real, double imag)
    {
        double zx = real;
        double zy = imag;
        int color_mod = 0, mod_value = rcsv.get_line_count() -1 ;
        for(int iterations = 0; iterations < max_iterations; iterations++)
        {
            double updated_zx = zx * zx - zy * zy + real;
            double updated_zy = 2.0 * zx * zy + imag;
            zx = updated_zx;
            zy = updated_zy;
            if(std::hypot(zx,zy) > 2.0)
            {
                color_mod = iterations % mod_value + 1;
                break;
            }
        }
        std::vector<unsigned char> rgba = rcsv.read_line(color_mod);
        return rgba;
    }

    std::vector<unsigned char> calc_rgb_standard(double real, double imag)
    {
        double zx = real;
        double zy = imag;
        int color_index = 0, num_colors = rcsv.get_line_count() -1;
        double factor = max_iterations / static_cast<double>(num_colors);
        for(int iterations = 0; iterations < max_iterations; iterations++)
        {
            double updated_zx = zx * zx - zy * zy + real;
            double updated_zy = 2.0 * zx * zy + imag;
            zx = updated_zx;
            zy = updated_zy;
            if(std::hypot(zx,zy) > 2.0)
            {
                color_index = iterations / factor + 1;
                break;
            }
        }
        std::vector<unsigned char> rgba = rcsv.read_line(color_index);
        return rgba;
    }
public:
    Mandelbrot(double center_real, double center_imag, double zoom_factor, int iterations)
    {
        this->center_real = center_real;
        this->center_imag = center_imag;
        this->zoom_factor = zoom_factor;
        this->max_iterations = iterations;
        start_real = center_real - (aspect_ratio/zoom_factor);
        start_imag = center_imag + (1/zoom_factor);
        image.resize(resolution_x*resolution_x*4);
    }

    void gen_fractal()
    {
        std::cout << "Starting generation of fractal" << std::endl;
        auto start_time = std::chrono::steady_clock::now();
        int num_threads = 20, rows_per_thread = resolution_y / num_threads;
        std::vector<std::thread> threads;
        for(int i = 0; i < num_threads; i++)
        {
            int start_y = i * rows_per_thread;
            int end_y = (i+1) * rows_per_thread;
            threads.emplace_back(&Mandelbrot::thread_gen_fractal,this,start_y,end_y);
        }
        for(auto &thread: threads)
            thread.join();
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
        std::cout << duration << " Seconds to generate fractal" << std::endl;
    }

    void thread_gen_fractal(int row_start, int row_end)
    {
        const double range = 2.0 / zoom_factor;
        double real_increment = (range*aspect_ratio)/((static_cast<double>((resolution_x)-1)));
        double imag_increment = (range)/(static_cast<double>(resolution_y)-1);


        for(int i = row_start; i < row_end; i++)
        {
            double imag_position = start_imag - i * imag_increment;
            for(int j = 0; j < resolution_x; j++)
            {
                double real_position = start_real + j * real_increment;
                // calculates rgb for complex cooridnate, and adds it to image buffer
                //std::vector<unsigned char> rgba = calc_rgb_modulo(real_position,imag_position);
                std::vector<unsigned char> rgba = calc_rgb_standard(real_position,imag_position);
                int index = ((i*resolution_x+j)*4);
                for(int k = 0; k < 4; k++) image[index+k] = rgba[k]; // adds rgba to image
            }
        }
    }

    void gen_png()
    {
        lodepng::encode(output_file, image, resolution_x, resolution_y);
    }
};
