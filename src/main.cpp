#include <iostream>
#include <string>

#include "application.hpp"

int main(int argc, char* argv[])
{
    //main.exe 0 my_name 3 80 0 0 0 10

    if (argc < 9) {
        std::cerr << "Usage: " << argv[0] << " <fractal_type> <file_name> <ls_iters> <line_length> <start_error> <end_error> <error_step> <count_iters_for_one_error>\n";
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    for(const auto& arg : args) {
        std::cout << "Argument ->  " << arg << std::endl;
    }

    InputData input_data
    {
        static_cast<fractal_type>(std::stoi(args.at(0))), // enum
        std::string(args.at(1)),                          // string
        std::stoi(args.at(2)),                            // int
        std::stoi(args.at(3)),                            // int
        std::stof(args.at(4)),                            // float
        std::stof(args.at(5)),                            // float
        std::stof(args.at(6)),                            // float
        std::stof(args.at(7))                             // float
    };

    auto application = Application{input_data};

    application.drawFractal();

    system("pause");
    return 0;
}
