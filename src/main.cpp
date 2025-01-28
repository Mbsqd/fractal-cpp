#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include "config.hpp"
#include "application.hpp"
#include "fractal.hpp"

int main()
{
    application_data_s app_config{};

    auto application = Application{app_config};
    // auto window = application.run();

    // bool fractalIsDrawAndSave = application.drawAndSaveFractal(app_config.FILENAME, app_config.START_ERROR_PERCENT);
    // if(!fractalIsDrawAndSave) {
    //     std::cerr << "Failed draw and save fractal" << std::endl;
    //     system("pause");
    //     return 1;
    // }

    bool fractalsIsDrawAndSave = application.drawAndSave100Fractals();
    if (!fractalsIsDrawAndSave) {
        std::cerr << "Failed draw and save fractals" << std::endl;
        system("pause");
        return 1;
    }

    system("pause");
    return 0;
}
