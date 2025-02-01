#ifndef Files_hpp
#define Files_hpp

#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "stb_image.h"

struct FractalDataForSheet {
    std::string title;
    int iterations;
    int lenght;
    std::unordered_map<std::string, double> th_dimens;
};

class Files {
public:
    Files() : filename("fractal") {
        sheet_full_file_string = output_path + filename + sheet_format;
    }
    Files(const std::string& filename) : filename(filename) {
        sheet_full_file_string = output_path + filename + sheet_format;
    }

    void updatePathByIters(float error, int iter) {
        std::string error_and_iter_str = "_err-" + std::to_string(error) + "_iter-" + std::to_string(iter);

        image_full_file_string = output_path + filename + error_and_iter_str + image_format;
        text_full_file_string = output_path + filename + error_and_iter_str + text_format;
    }

    bool saveLSystemToFile(const std::vector<char>& instructions) {
        std::ofstream file(text_full_file_string);
        if (!file) {
            std::cerr << "Failed saved l-system to " << text_full_file_string << std::endl;
            return false;
        }

        for(char c : instructions) {
            file << c;
        }

        file.close();
        return true;
    }

    std::vector<char> readLSystemFromFile() {
        std::ifstream file(text_full_file_string);
        if(!file) return {};

        std::vector<char> instructions;
        char c;
        while(file.get(c)) {
            if(c != '\n' && c != '\r') instructions.push_back(c);
        }

        file.close();
        return instructions;
    }

    bool saveImage(sf::Image& image) {
        bool result = image.saveToFile(image_full_file_string);

        if (!result) {
            std::cerr << "Failed save image to " << image_full_file_string << std::endl;
            return false;
        }

        std::cout << "Image saved  to " << image_full_file_string << std::endl;
        return true;
    }

    std::vector<std::vector<bool>> readBmpWithSTB(int& width, int& height) {
        int channels;
        unsigned char* data = stbi_load(image_full_file_string.c_str(), &width, &height, &channels, 3); // Загружаем как RGB (3 канала)
        if (!data) {
            std::cerr << "Failed to load image: " << filename << std::endl;
            return {};
        }

        std::vector<std::vector<bool>> binaryImage(height, std::vector<bool>(width, false));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int pixelIndex = (y * width + x) * 3; // RGB
                unsigned char r = data[pixelIndex];
                unsigned char g = data[pixelIndex + 1];
                unsigned char b = data[pixelIndex + 2];

                binaryImage[y][x] = (r + g + b < 128 * 3);
            }
        }

        stbi_image_free(data);
        return binaryImage;
    }

    std::string getFullImageFileString() {
        return image_full_file_string;
    }

    void writeManyToCsvFile(std::ofstream &out_file, std::vector<double>& dimensions_vector, float current_error_percent, FractalDataForSheet fractal_data) {

        std::vector<double> avarage_dimension, minimal_dimension, maximum_dimension {};
        double avarage, minimal, maximum = 0;

        for(auto dimension : dimensions_vector) {
            if (dimension > fractal_data.th_dimens["th_max_dimen"]) {
                maximum_dimension.push_back(dimension);
                // maximum += dimension;
            } else if (dimension < fractal_data.th_dimens["th_min_dimen"]) {
                minimal_dimension.push_back(dimension);
                // minimal += dimension;
            } else {
                avarage_dimension.push_back(dimension);
                avarage += dimension;
            }
        }

        for(int i = 0; i < minimal_dimension.size(); i++) {
            if (i == 0) {
                 minimal = minimal_dimension.at(i);
            }

            if(minimal > minimal_dimension.at(i)) minimal = minimal_dimension.at(i);
        }

        for(int i = 0; i < maximum_dimension.size(); i++) {
            if (i == 0) {
                 maximum = maximum_dimension.at(i);
            }

            if(maximum < maximum_dimension.at(i)) maximum = maximum_dimension.at(i);
        }

        avarage = avarage_dimension.size() == 0 ? 0 : avarage / avarage_dimension.size();
        minimal = minimal_dimension.size() == 0 ? 0 : minimal;
        maximum = maximum_dimension.size() == 0 ? 0 : maximum;


        out_file << fractal_data.title << ","
                << fractal_data.iterations << ","
                << fractal_data.lenght << ","
                << fractal_data.th_dimens["th_avg_dimen"] << ","
                << (avarage_dimension.empty() ? "" : std::to_string(avarage)) << ","
                << (minimal_dimension.empty() ? "" : std::to_string(minimal)) << ","
                << (maximum_dimension.empty() ? "" : std::to_string(maximum)) << ","
                << current_error_percent << "\n";
    }

    std::string getFullSheetFileString() {
        return sheet_full_file_string;
    }

private:
    const std::string filename;

    const std::string output_path = "./output/";

    const std::string image_format = ".bmp";
    const std::string text_format = ".txt";
    const std::string sheet_format = ".csv";

    std::string image_full_file_string;
    std::string text_full_file_string;
    std::string sheet_full_file_string;
};

#endif
