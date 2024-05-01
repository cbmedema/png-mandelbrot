//
// Created by jassange on 4/26/24.
//

#ifndef MANDELBROT_H
#define MANDELBROT_H

#endif //MANDELBROT_H

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include "lodepng.h"
#include <thread>
#include <chrono>

class Mandelbrot
{
private:
    const std::string color_map_file = "color_map.csv";
    const std::string output_file = "output.png";
    // device resolution variables
    const int resolution_x = 7680;
    const int resolution_y = 4800;
    const double aspect_ratio = resolution_x / static_cast<double>(resolution_y);
    std::vector<unsigned char> image;

    // fractal variables
    double center_real, center_imag, zoom_factor;
    int max_iterations;
    double start_real, start_imag;

    // fractal private methods
    std::vector<unsigned char> read_csv(int line_number)
    {
        std::ifstream file(color_map_file);
        std::string line;
        // goes to line in file to get color
        for(int i = 0; i <= line_number; i++)
        {
            std::getline(file,line);
        }
        std::vector<unsigned char> rgb(3); // unsigned char used to store each rgb value in single byte
        std::stringstream ss(line);
        std::string color, endptr;
        // converts csv line to seperate rgb values
        for(int i = 0; i < 3; i++)
        {
            std::getline(ss,color,',');
            rgb[i] = static_cast<unsigned char>(std::stoi(color));
        }
        return rgb;
    }
    std::vector<unsigned char> calc_rgba_modulo(double real, double imag)
    {
        double zx = real;
        double zy = imag;
        int color_mod = 0;
        for(int iterations = 0; iterations < max_iterations; iterations++)
        {
            double updated_zx = zx * zx - zy * zy + real;
            double updated_zy = 2.0 * zx * zy + imag;
            zx = updated_zx;
            zy = updated_zy;
            if(std::hypot(zx,zy) > 2.0)
            {
                color_mod = iterations % 14 + 1;
                break;
            }
        }
        std::vector<unsigned char> rgba = read_csv(color_mod);
        rgba.push_back(255); // adds alpha value to rgb
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
                std::vector<unsigned char> rgba = calc_rgba_modulo(real_position,imag_position);
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