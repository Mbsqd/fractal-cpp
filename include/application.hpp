#ifndef Application_hpp
#define Application_hpp

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>

#include "fractal.hpp"

class Application {
public:
    Application(const application_data_s& app_config) : axiom(app_config.AXIOM), rules(app_config.RULES), error_percent_list(app_config.ERRORS_PERCENT_LIST) {
        win_size_x = app_config.window_config.WIN_SIZE_X;
        win_size_y = app_config.window_config.WIN_SIZE_Y;
        max_framerate = app_config.window_config.MAX_FRAMERATE;
        window_name = app_config.window_config.WIN_NAME;

        iterations = app_config.ITERATIONS;
        angle = app_config.ANGLE;
        length = app_config.LENGTH;
        start_error_percent = app_config.START_ERROR_PERCENT;
        fractal_max_size = app_config.FRACTAL_MAX_SIZE;
        filename = app_config.FILENAME;
    }

    sf::RenderWindow run() {
        auto window = sf::RenderWindow(sf::VideoMode({win_size_x, win_size_y}), window_name);
        window.setFramerateLimit(max_framerate);
        return window;
    }

    bool drawAndSaveFractal(std::string filename, float err_percent) {
        std::vector<char> instructions = generateLSystem(iterations, axiom, rules, err_percent);
        BoundingBox calculatedFractalSize = computeBoundingBox(instructions, angle, length);

        if(calculatedFractalSize.max.x > fractal_max_size || calculatedFractalSize.max.y > fractal_max_size) {
            std::cerr << "Fractal have size more than " << fractal_max_size  << std::endl;
            return false;
        } else if (calculatedFractalSize.max.x < 0 || calculatedFractalSize.max.y < 0) {
            std::cerr << "Fractal have size less than " << 0  << std::endl;
            return false;
        }

        sf::Vector2u calculatedImageSize(calculatedFractalSize.max.x - calculatedFractalSize.min.x, calculatedFractalSize.max.y - calculatedFractalSize.min.y);

        bool isSavedInstruction = saveLSystemToFile(instructions, "./output/" + filename + ".txt");
        if (!isSavedInstruction) {
            std::cerr << "Failed saved l-system to " << filename << ".txt file" << std::endl;
            return false;
        }

        sf::RenderTexture texture(calculatedImageSize);
        // sf::RenderTexture texture2({fractal_max_size, fractal_max_size}); Генерация фрактала без обрезки

        sf::Vector2f calculatedStartPositon = calculateStartPosition(texture, instructions, calculatedFractalSize, angle, length);
        drawLSystem(texture, instructions, angle, length, calculatedStartPositon, start_angle);
        // drawLSystem(texture2, instructions, angle, length, calculatedStartPositon, start_angle); // Генерация фрактала без обрезки

        sf::Texture result = texture.getTexture();
        sf::Image image = result.copyToImage();

        // sf::Texture result2 = texture2.getTexture(); // Генерация фрактала без обрезки
        // sf::Image image2 = result2.copyToImage(); // Генерация фрактала без обрезки

        bool isSavedImage = image.saveToFile("./output/" + filename + ".bmp");
        // bool isSavedImage2 = image2.saveToFile("./output/" + filename + "__2.bmp"); // Генерация фрактала без обрезки

        if (!isSavedImage) {
            std::cerr << "Failed save image to " << filename << ".bmp" << std::endl;
            return false;
        }

        std::cout << "File saved to " << filename << ".bmp" << std::endl;
        return true;
    }

    bool drawAndSave100Fractals() {
        for(int i = 0; i < error_percent_list.size(); i++) {
            float current_error_percent = error_percent_list[i];
            std::cout << "Index: " << i << ", Error percent: " << current_error_percent << "%" << std::endl;

            for(int j = 0; j < 2; j++) {
                bool fractalIsDrawAndSave = drawAndSaveFractal(filename + "_err-" + std::to_string(current_error_percent) + "_it-" + std::to_string(j), current_error_percent);
                if (!fractalIsDrawAndSave) return false;
            }
        }
        return true;
    }

    double getFractalDimension() {
        int width = 0;
        int height = 0;

        std::cout << "Try open: ./output/" << filename << ".bmp" << std::endl;
        auto image = readBmp("./output/" + filename + ".bmp", width, height);
        if(image.empty()) {
             std::cerr <<"Failed open: ./output/" + filename + ".bmp file" << std::endl;
        }

        double fractalDim = calculateFractalDimension(image);

        return fractalDim;
    }

private:
    unsigned int win_size_x;
    unsigned int win_size_y;
    unsigned int max_framerate;
    std::string window_name;

    std::string filename;
    int iterations;
    float angle;
    float length;
    unsigned int fractal_max_size;
    float start_angle;
    float start_error_percent;

    const std::vector<float>& error_percent_list;
    const std::vector<char>& axiom;
    const std::unordered_map<char, std::vector<char>>& rules;
};

#endif
