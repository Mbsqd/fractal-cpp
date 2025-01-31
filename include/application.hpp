#ifndef Application_hpp
#define Application_hpp

#include <vector>

#include "fractal.hpp"

enum class fractal_type {
    koha_snowflake = 0,
    dragon_curve,
    sierpinski_curve,
    levy_curve,
    square_island_of_koha,
};

struct InputData {
    fractal_type fractal_name = fractal_type::koha_snowflake;
    std::string file_name = "file";

    int ls_iters = 3;
    int line_lenght = 80;

    float start_error = 0;
    float end_error = 0;
    float error_step = 0;
    float count_iters_for_one_error = 0;
};

class Application {
public:
    Application(const InputData& input_data) : cfg{} {
        std::cout<< input_data.file_name << std::endl;
        cfg.LSYSTEM_ITERATIONS = input_data.ls_iters;
        cfg.LINE_LENGTH = input_data.line_lenght;
        cfg.COUNT_ITERS_FOR_ONE_ERROR = input_data.count_iters_for_one_error;
        cfg.FILENAME = input_data.file_name;
        cfg.ERRORS_PERCENT_LIST = generateErrorList(input_data.start_error, input_data.end_error, input_data.error_step);

        // Название фрактала
        switch (input_data.fractal_name) {
            case fractal_type::koha_snowflake:
                cfg.FRACTAL_NAME = "Koch Snowflake";
                cfg.AXIOM = {'F', '-', '-', 'F', '-', '-', 'F'};
                cfg.RULES = {{'F', {'F', '+', 'F', '-', '-', 'F', '+', 'F'}}};
                cfg.FRACTAL_ANGLE = 60;
                break;
            case fractal_type::dragon_curve:
                cfg.FRACTAL_NAME = "Dragon Curve";
                cfg.AXIOM = {'F', 'X'};
                cfg.RULES = {{'X', {'X', '+', 'Y', 'F', '+'}}, {'Y', {'-', 'F', 'X', '-', 'Y'}}};
                cfg.FRACTAL_ANGLE = 90;
                break;
            case fractal_type::sierpinski_curve:
                cfg.FRACTAL_NAME = "Sierpinski Curve";
                cfg.AXIOM = {'F', '+', 'X', 'F', '+', 'F', '+', 'X', 'F'};
                cfg.RULES = {{'X', {'X', 'F', '-', 'F', '+', 'F', '-', 'X', 'F', '+', 'F', '+', 'X', 'F', '-', 'F', '+', 'F', '-', 'X'}}};
                cfg.FRACTAL_ANGLE = 90;
                break;
            case fractal_type::levy_curve:
                cfg.FRACTAL_NAME = "Levy Curve";
                cfg.AXIOM = {'F'};
                cfg.RULES = {{'F', {'+', 'F', '-', '-', 'F', '+'}}};
                cfg.FRACTAL_ANGLE = 45;
                break;
            case fractal_type::square_island_of_koha:
                cfg.FRACTAL_NAME = "Square Island of Koch";
                cfg.AXIOM = {'F', '+', 'F', '+', 'F', '+', 'F'};
                cfg.RULES = {{'F', {'F', '-', 'F', '+', 'F', '+', 'F', 'F', 'F', '-', 'F', '-', 'F', '+', 'F'}}};
                cfg.FRACTAL_ANGLE = 90;
                break;
        }
    }

    void drawFractal() {
        std::cout<< "Start draw fractal" << std::endl;
        Fractal fractal {cfg};
        bool result = fractal.drawAndSaveFractalIters();
        if(!result) {
            std::cerr << "Error create and draw fractal" << std::endl;
            return;
        }
    }

private:
    application_data_s cfg;

    std::vector<float> generateErrorList(float start, float end, float step) {
        std::vector<float> errors;

        start = abs(start);
        end = abs(end);
        step = abs(step);

        if(start == end) {
            errors.push_back(start);
            return errors;
        } else if (end < start) {
            std::cout << "Start value = end, because end value < start";
            errors.push_back(end);
            return errors;
        } else if (end - start < step) {
            std::cout << "Start value = only start value, because step so small";
            errors.push_back(start);
            return errors;
        }

        for (float e = start; e <= end; e += step) {
            errors.push_back(e);
        }
        return errors;
    }
};

#endif
