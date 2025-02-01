#ifndef Fractal_hpp
#define Fractal_hpp

#include <SFML/Graphics.hpp>

#include <string>
#include <unordered_map>
#include <cmath>
#include <random>
#include <cstring>
#include <algorithm>

#include "config.hpp"
#include "files.hpp"

struct BoundingBox {
    sf::Vector2f min, max;
};

class Fractal {
public:
    Fractal(application_data_s& cfg) :
        ls_iterations(cfg.LSYSTEM_ITERATIONS), work_angle(cfg.FRACTAL_ANGLE), start_angle(cfg.START_ANGLE), line_lenght(cfg.LINE_LENGTH),
        errors_list(cfg.ERRORS_PERCENT_LIST), th_dimens(cfg.THEORETICAL_DIMENSIONS), count_iters_for_one_error(cfg.COUNT_ITERS_FOR_ONE_ERROR),
        title(cfg.FRACTAL_NAME), filename(cfg.FILENAME), axiom(cfg.AXIOM), rules(cfg.RULES), fractal_max_size(cfg.FRACTAL_MAX_SIZE), files(filename) {}

    std::vector<char> generateLSystem() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 100.0f);

        std::vector<char> current = axiom;
        for (int i = 0; i < ls_iterations; ++i) {
            std::vector<char> next;
            for (char c : current) {
                if(dist(gen) < current_error_percent) continue;  // если число меньше errorPercent пропустить символ

                if (rules.count(c)) {
                    next.insert(next.end(), rules.at(c).begin(), rules.at(c).end()); // Добавляем правило
                } else {
                    next.push_back(c); // Оставляем символ без изменений
                }
            }
            current = std::move(next);
        }

        return current;
    }

    void drawLSystem(sf::RenderTexture& texture, std::vector<char>& instructions, float dls_start_angle, sf::Vector2f dls_start_position) {
        BoundingBox fractal_size = computeBoundingBox(instructions);

        std::vector<std::pair<sf::Vector2f, float>> stack;
        sf::VertexArray lines(sf::PrimitiveType::Lines);

        for (char c : instructions) {
            if (c == 'F') {
                sf::Vector2f newPosition = dls_start_position + sf::Vector2f(cos(dls_start_angle * 3.14159 / 180) * line_lenght, sin(dls_start_angle * 3.14159 / 180) * line_lenght);
                lines.append(sf::Vertex({dls_start_position, sf::Color::Black}));
                lines.append(sf::Vertex({newPosition, sf::Color::Black}));
                dls_start_position = newPosition;
            } else if (c == '+') {
                dls_start_angle += work_angle;
            } else if (c == '-') {
                dls_start_angle -= work_angle;
            } else if (c == '[') {
                stack.emplace_back(dls_start_position, dls_start_angle);
            } else if (c == ']') {
                auto state = stack.back();
                stack.pop_back();
                dls_start_position = state.first;
                dls_start_angle = state.second;
            }
        }

        texture.clear(sf::Color::White);
        texture.draw(lines);
        texture.display();
    }

double calculateFractalDimension(const std::vector<std::vector<bool>>& image) {
    int width = image[0].size();
    int height = image.size();
    int minL = std::min(width, height);
    int maxN = static_cast<int>(std::floor(minL * 0.2));

    std::vector<int> ns(10);
    std::vector<int> countResults(10);

    for (int i = 0, n = maxN; i < 10; i++) {
        ns[i] = n;
        n = std::max(1, static_cast<int>(std::floor(n / 1.3))); // Защита от нуля
    }

    for (int i = 0; i < 10; i++) {
        if (ns[i] <= 0) break; // Если n стало 0, выходим из цикла
        countResults[i] = countBoxes(image, ns[i]);
    }

    return processArrays(ns, countResults);
}

    double getFractalDimension(std::string full_filename) {
        int width = 0;
        int height = 0;

        std::vector<std::vector<bool>> image = files.readBmpWithSTB(width, height);
        if(image.empty()) {
             std::cerr <<"Failed open: " << files.getFullImageFileString() << std::endl;
             return 0;
        }
        std::cout << "BMP file is open" << std::endl;
        double fractalDim = calculateFractalDimension(image);

        return fractalDim;
    }

    bool drawAndSaveFractal(std::vector<double>& fractal_dimensions_vector, std::ofstream& out_csv_file, float error, int iter) {
        files.updatePathByIters(error, iter);
        std::vector<char> instructions = generateLSystem();

        BoundingBox fractal_size = computeBoundingBox(instructions);
        if (fractal_size.max.x <= fractal_size.min.x || fractal_size.max.y <= fractal_size.min.y) {
            std::cerr << "Bounding box has invalid size!" << std::endl;
            return false;
        } else if(fractal_size.max.x > fractal_max_size || fractal_size.max.y > fractal_max_size) {
            std::cerr << "Fractal have size more than " << fractal_max_size  << std::endl;
            return false;
        } else if (fractal_size.max.x < 0 || fractal_size.max.y < 0) {
            std::cerr << "Fractal have size less than " << 0  << std::endl;
            return false;
        }

        std::cout << "FR: Max x: " << fractal_size.max.x << " y: " << fractal_size.max.y;
        std::cout << " Min x: " << fractal_size.min.x << " y: " << fractal_size.min.y << std::endl;

        bool isSavedInstruction = files.saveLSystemToFile(instructions);
        if (!isSavedInstruction) return false;

        sf::Vector2u calculatedImageSize(fractal_size.max.x - fractal_size.min.x, fractal_size.max.y - fractal_size.min.y);
        if (calculatedImageSize.x <= 0 || calculatedImageSize.y <= 0) {
            std::cerr << "Error: calculatedImageSize is zero (" << calculatedImageSize.x << "x" << calculatedImageSize.y << ")" << std::endl;
            return false;
        }

        sf::RenderTexture texture(calculatedImageSize);
        std::cout << "Image width: " << calculatedImageSize.x << ", height: " << calculatedImageSize.y << std::endl;

        sf::Vector2f calculatedStartPositon = calculateStartPosition(texture, instructions);

        std::cout<< "Start x: " << calculatedStartPositon.x << " y: " << calculatedStartPositon.y << std::endl;

        drawLSystem(texture, instructions, start_angle, calculatedStartPositon); ///////////////////////////////// maybe work_angle?

        sf::Texture result = texture.getTexture();
        sf::Image image = result.copyToImage();

        bool isSavedImage = files.saveImage(image);
        if (!isSavedImage) return false;

        double fractal_dimension = 0;
        fractal_dimension = getFractalDimension(filename);

        if(fractal_dimension == 0) {
            std::cerr << "Failed calculate fractal dimension" << std::endl;
            return false;
        }

        std::cout<<"Fractal dimension: " << fractal_dimension << std::endl;

        fractal_dimensions_vector.push_back(fractal_dimension);

        return true;
    }

    bool drawAndSaveFractalIters() {
        std::ofstream out_csv_file(files.getFullSheetFileString());
        if(!out_csv_file) {
             std::cerr << files.getFullSheetFileString() << std::endl;
             return false;
        }
        out_csv_file << "FractalName,Iterations,Line lenght, Theoretical Dimension, Avarage, Minimal, Maximum, Error Percent\n";

        FractalDataForSheet fractal_data{title, ls_iterations, line_lenght, th_dimens};

        std::vector<double> fractal_dimensions_vector {};

        for(int i = 0; i < errors_list.size(); i++) {
            current_error_percent = errors_list[i];

            for(int j = 1; j <= count_iters_for_one_error; j++) {
                std::cout << "Index: " << j << ", Error percent: " << current_error_percent << "%" << std::endl;
                bool fractalIsDrawAndSave = drawAndSaveFractal(fractal_dimensions_vector, out_csv_file, current_error_percent, j);
                if (!fractalIsDrawAndSave) continue;;
            }

            files.writeManyToCsvFile(out_csv_file, fractal_dimensions_vector, current_error_percent, fractal_data);
        }

        out_csv_file.close();
        std::cout<<"CSV file saved to: " << files.getFullSheetFileString() << std::endl;

        return true;
    }



private:
    int ls_iterations;
    int line_lenght;
    float work_angle;
    float start_angle;
    float current_error_percent;
    float count_iters_for_one_error;
    int fractal_max_size;
    const std::vector<float> errors_list;

    std::unordered_map<std::string, double> th_dimens;

    const std::string title;
    const std::string filename;
    const std::vector<char> axiom;
    const std::unordered_map<char, std::vector<char>> rules;

    BoundingBox fractal_size;

    Files files;

    BoundingBox computeBoundingBox(const std::vector<char>& instructions) {
        sf::Vector2f position{0, 0}; // Начальная позиция (потом скорректируем)
        float current_angle = start_angle;
        std::vector<std::pair<sf::Vector2f, float>> stack;

        sf::Vector2f min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        sf::Vector2f max = {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};

        for (char c : instructions) {
            if (c == 'F') {
                sf::Vector2f newPosition = position + sf::Vector2f(std::cos(current_angle * 3.14159f / 180) * line_lenght,
                                                                std::sin(current_angle * 3.14159f / 180) * line_lenght);
                position = newPosition;
            } else if (c == '+') {
                current_angle += work_angle;
            } else if (c == '-') {
                current_angle -= work_angle;
            } else if (c == '[') {
                stack.emplace_back(position, current_angle);
            } else if (c == ']') {
                auto state = stack.back();
                stack.pop_back();
                position = state.first;
                current_angle = state.second;
            }

            // Обновляем границы
            if (position.x < min.x) min.x = position.x;
            if (position.y < min.y) min.y = position.y;
            if (position.x > max.x) max.x = position.x;
            if (position.y > max.y) max.y = position.y;
        }


        return {min, max};
    }

    sf::Vector2f calculateStartPosition(sf::RenderTexture& texture, const std::vector<char>& instructions) {
        BoundingBox bbox = computeBoundingBox(instructions);

        sf::Vector2f size = bbox.max - bbox.min;
        sf::Vector2f imageSize(texture.getSize().x, texture.getSize().y);

        float scale = 1.0f; // Проверяем, чтобы не было деления на ноль
        if (size.x > 0 && size.y > 0) {
            scale = std::min(imageSize.x / size.x, imageSize.y / size.y);
        }

        sf::Vector2f start_position = (imageSize - size * scale) * 0.5f - bbox.min * scale; // Центрируем фрактал в изображении
        return start_position;
    }

    double processArrays(const std::vector<int>& ns, const std::vector<int>& countResults) {
        std::vector<double> logN, logE;

        for (size_t i = 0; i < ns.size(); i++) {
            if (countResults[i] == 0) continue; // logN.push_back(std::log(countResults[i])); // before
            logN.push_back(std::log(countResults[i] + 1e-10)); // добавляем 1e-10 вместо пропуска
            logE.push_back(std::log(1.0 / ns[i]));
        }

        double n = static_cast<double>(logN.size());
        if (n < 2) return 0;

        long double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
        for (int i = 0; i < n; i++) {
            sumX += logE[i];
            sumY += logN[i];
            sumXY += logE[i] * logN[i];
            sumXX += logE[i] * logE[i];
        }

        return (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
    }

    int countBoxes(const std::vector<std::vector<bool>>& image, int boxSize) {
        int height = image.size();
        int width = image[0].size();
        int count = 0;

        for (int y = 0; y < height; y += boxSize) {
            for (int x = 0; x < width; x += boxSize) {
                bool hasFractal = false;
                for (int dy = 0; dy < boxSize && (y + dy) < height; ++dy) {
                    for (int dx = 0; dx < boxSize && (x + dx) < width; ++dx) {
                        if (image[y + dy][x + dx]) {
                            hasFractal = true;
                            break;
                        }
                    }
                    if (hasFractal) break;
                }
                if (hasFractal) count++;
            }
        }
        return count;
    }

};

#endif
