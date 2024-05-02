//
// Created by bryce on 5/1/24.
//

#ifndef READ_CSV_H
#define READ_CSV_H

#endif //READ_CSV_H
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>

class ReadCSV {
private:
    int line_count;
    std::unordered_map<int,std::vector<unsigned char>> color_map;

    void convert_to_color_map(std::string file) {
        std::ifstream f(file);
        if(!f.is_open()) { // exits if file can not be opened
            std::cout << "ERROR: File not found!";
            std::exit(-1);
        }
        std::cout << "Generating hash table for color map " << std::endl;
        line_count = 0;
        std::string color, line;
        while(std::getline(f,line)) {
            std::stringstream ss(line);
            std::vector<unsigned char> rgb(4);
            for(int i = 0; i < 3; i++) {
                std::getline(ss,color,',');
                rgb[i] = static_cast<unsigned char>(std::stoi(color));
            }
            rgb[3] = 255; // sets alpha channel
            color_map[line_count] = rgb;
            line_count++;
        }
        f.close();
        std::cout << "Hash table successfully created!" << std::endl;
    }

public:
    ReadCSV(std::string file){
        convert_to_color_map(file);
    }

    std::vector<unsigned char> read_line(int line_number) {
        return color_map[line_number];
    }

    int get_line_count() {
        return line_count;
    }

};
