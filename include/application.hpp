#ifndef Application_hpp
#define Application_hpp

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>

class Application {
public:
    Application(unsigned int _win_size_x, unsigned int _win_size_y, unsigned int _max_framerate, std::string _window_name)
        : win_size_x(_win_size_x), win_size_y(_win_size_y), max_framerate(_max_framerate), window_name(_window_name) {}

    sf::RenderWindow run() {
        auto window = sf::RenderWindow(sf::VideoMode({win_size_x, win_size_y}), window_name);
        window.setFramerateLimit(max_framerate);
        return window;
    }

    bool saveLSystemToFile(const std::vector<char>& instructions, const std::string& filename) {
        std::ofstream file(filename);
        if (!file) return false;

        for(char c : instructions) {
            file << c;
        }

        file.close();
        return true;
    }

    std::vector<char> readLSystemFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if(!file) return {};

        std::vector<char> instructions;
        char c;
        while(file.get(c)) {
            if(c != '\n' && c != '\r') instructions.push_back(c);
        }

        file.close();
        return instructions;
    }

private:
    unsigned int win_size_x;
    unsigned int win_size_y;
    unsigned int max_framerate;
    std::string window_name;
};

#endif
