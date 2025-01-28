#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include "application.hpp"
#include "fractal.hpp"

const unsigned int WIN_SIZE_X = 1024;
const unsigned int WIN_SIZE_Y = 768;
const unsigned int MAX_FRAMERATE = 144;
const std::string WIN_NAME = "Fractall cpp app";

const int ITERATIONS = 3;
const float ANGLE = 60;
const float LENGTH = 80;
const int FRACTAL_MAX_SIZE = 10000;
const std::string AXIOM = "F--F--F";
const std::vector<char> AXIOM_V = {'F','-','-','F','-','-','F'};
const std::unordered_map<char, std::string> RULES = { {'F', "F+F--F+F"} };
const std::unordered_map<char, std::vector<char>> RULES_V = { {'F', {'F','+','F','-','-','F','+','F'}} };

const sf::Vector2f START_POSITION(0, 0);
const float START_ANGLE = 60;

int main()
{
    auto application = Application{WIN_SIZE_X, WIN_SIZE_Y, MAX_FRAMERATE, WIN_NAME};
    auto window = application.run();

    std::vector<char> instructions = generateLSystem(ITERATIONS, AXIOM_V, RULES_V);
    BoundingBox calculatedFractalSize = computeBoundingBox(instructions, ANGLE, LENGTH);
    if(calculatedFractalSize.max.x > FRACTAL_MAX_SIZE || calculatedFractalSize.max.y > FRACTAL_MAX_SIZE) {
        std::cerr << "Fractal have size more than " << FRACTAL_MAX_SIZE  << std::endl;
    } else if (calculatedFractalSize.max.x < 0 || calculatedFractalSize.max.y < 0) {
        std::cerr << "Fractal have size less than " << 0  << std::endl;
    }

    bool isSavedInstruction = application.saveLSystemToFile(instructions, "./output/lsystem.txt");
    if (!isSavedInstruction) {
        std::cerr << "Failed saved l-system txt file" << std::endl;
        return 1;
    }

    sf::RenderTexture texture({FRACTAL_MAX_SIZE, FRACTAL_MAX_SIZE});

    sf::Vector2f calculatedStartPositon = calculateStartPosition(texture, instructions, calculatedFractalSize, ANGLE, LENGTH);
    drawLSystem(texture, instructions, ANGLE, LENGTH, calculatedStartPositon, START_ANGLE);

    sf::Texture result = texture.getTexture();
    sf::Image image = result.copyToImage();

    bool isSavedImage = image.saveToFile("./output/fractal.bmp");
    if (isSavedImage) {
        std::cout << "File saved to fractal.png" << std::endl;
        return 0;
    } else {
        std::cerr << "Failed save image" << std::endl;
    }
}
