#ifndef Config_hpp
#define Config_hpp

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>

struct application_data_s {
    int LSYSTEM_ITERATIONS = 3;
    float FRACTAL_ANGLE = 60;
    float LINE_LENGTH = 80;
    int FRACTAL_MAX_SIZE = 10000;
    float START_ANGLE = 60;
    float COUNT_ITERS_FOR_ONE_ERROR = 10;
    std::vector<float> ERRORS_PERCENT_LIST = {};
    float MISTAKE = 2.5;

    double THEORETICAL_AVG_DIMENSION = std::log(4)/std::log(3);
    double THEORETICAL_MIN_DIMENSION = THEORETICAL_AVG_DIMENSION - (THEORETICAL_AVG_DIMENSION / 100 * MISTAKE);
    double THEORETICAL_MAX_DIMENSION = THEORETICAL_AVG_DIMENSION + (THEORETICAL_AVG_DIMENSION / 100 * MISTAKE);

    std::unordered_map<std::string, double> THEORETICAL_DIMENSIONS =
    {
        {"th_avg_dimen", {THEORETICAL_AVG_DIMENSION}},
        {"th_min_dimen", {THEORETICAL_MIN_DIMENSION}},
        {"th_max_dimen", {THEORETICAL_MAX_DIMENSION}}
    };

    std::string FRACTAL_NAME = "Koha";
    std::string FILENAME = "fractal";
    std::vector<char> AXIOM = {'F','-','-','F','-','-','F'};
    std::unordered_map<char, std::vector<char>> RULES = { {'F', {'F','+','F','-','-','F','+','F'}} };
};

#endif
