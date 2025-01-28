#ifndef Config_hpp
#define Config_hpp

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>

struct window_data_s {
    unsigned int WIN_SIZE_X = 1024;
    unsigned int WIN_SIZE_Y = 768;
    unsigned int MAX_FRAMERATE = 144;
    std::string WIN_NAME = "FractalCpp";
};

struct application_data_s {
    window_data_s window_config{};
    int ITERATIONS = 3;
    float ANGLE = 60;
    float LENGTH = 80;
    float START_ERROR_PERCENT = 0;
    int FRACTAL_MAX_SIZE = 10000;
    std::vector<float> ERRORS_PERCENT_LIST = { 0, 3, 5, 10, 15, 20, 25 };
    std::vector<char> AXIOM = {'F','-','-','F','-','-','F'};
    std::unordered_map<char, std::vector<char>> RULES = { {'F', {'F','+','F','-','-','F','+','F'}} };
    std::string FILENAME = "fractal";

    sf::Vector2f START_POSITION{0, 0};
    float START_ANGLE = 60;
};

#endif
