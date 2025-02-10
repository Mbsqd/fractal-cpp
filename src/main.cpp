#include <iostream>
#include <string>
#include <map>

#include "application.hpp"


bool configure(int& argc, char** argv, InputData& input_data) {
    if (argc < 10) {
        std::cerr << "Usage: " << argv[0] << " <fractal_type> <file_name> <ls_iters> <line_length> <start_error> <end_error> <error_step> <mistake> <count_iters_for_one_error> \n";
        return false;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    for(const auto& arg : args) {
        std::cout << "Argument ->  " << arg << std::endl;
    }

    input_data = InputData
    {
        static_cast<fractal_type>(std::stoi(args.at(0))), // enum
        std::string(args.at(1)),                          // string
        std::stoi(args.at(2)),                            // int
        std::stoi(args.at(3)),                            // int
        std::stof(args.at(4)),                            // float
        std::stof(args.at(5)),                            // float
        std::stof(args.at(6)),                            // float
        std::stof(args.at(7)),                            // float
        std::stoi(args.at(8)),                            // int
    };

    return true;
}

std::map<int, std::string> fractal_names = {
    {0, "koha_snowflake"},
    {1, "dragon_curve"},
    {2, "sierpinski_curve"},
    {3, "levy_curve"},
    {4, "square_island_of_koha"},
};

bool experement() {
    std::cout<<"Start experement" << std::endl;
    std::ofstream out_csv_file("./output/results3.csv");
    if (!out_csv_file.is_open()) {
        std::cout<<"Failed open file ./output/results3.csv" << std::endl;
        return false;
    }

    out_csv_file << "FractalName,Iterations,Line lenght, Theoretical Dimension, Avarage, Minimal, Maximum\n";

    for (int iters = 10; iters <= 12; iters++) {
    int counter = 10;
        for(int lenght = 20; lenght <= 100; lenght += counter) {

            std::string filename = fractal_names.at(3) + "_iters-" + std::to_string(iters) + "_lenght-" + std::to_string(lenght);

            InputData exp_data{ static_cast<fractal_type>(3), filename, iters, lenght, 0.0f, 0.0f, 0.0f, 1};
            auto app = Application(exp_data);
            app.drawFractal();
        }
    }

    out_csv_file.close();

    std::cout<<"End experement" << std::endl;
    return true;
}

int main(int argc, char* argv[])
{
    //main.exe 0 my_name 3 80 0 0 0 1 10

    // mkdir build
    // cd build
    // cmake -DCMAKE_BUILD_TYPE=Release ..
    // cmake --build . --config Release

    // bool result = experement();
    // if(!result) {
    //     std::cerr << "Failed experement" << std::endl;
    //     return 1;
    // }

    InputData input_data;
    bool result = configure(argc, argv, input_data);
    if(!result) {
        std::cout << "Failed configuration" << std::endl;
        return 1;
    }

    auto application = Application{input_data};
    application.drawFractal();

    system("pause");
    return 0;
}
