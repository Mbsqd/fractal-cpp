#ifndef Fractal_hpp
#define Fractal_hpp

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <cmath>
#include <random>

struct BoundingBox {
    sf::Vector2f min, max;
};

std::vector<char> generateLSystem(int iterations, const std::vector<char>& axiom, const std::unordered_map<char, std::vector<char>>& rules, float errorPercent) {
    std::cout<<"Before memo Error -> " << errorPercent << std::endl;

    static std::unordered_map<int, std::vector<char>> memo; // Кэш для мемоизации

    static float err_percent_old = errorPercent;
    if(err_percent_old != errorPercent) {
        memo.clear();
    }

    if (memo.count(iterations)) return memo[iterations]; // Используем кэш

    std::cout<<"After memo Error -> " << errorPercent << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 100.0f);

    std::vector<char> current = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::vector<char> next;
        for (char c : current) {
            if(dist(gen) < errorPercent) continue;  // если число меньше errorPercent пропустить символ

            if (rules.count(c)) {
                next.insert(next.end(), rules.at(c).begin(), rules.at(c).end()); // Добавляем правило
            } else {
                next.push_back(c); // Оставляем символ без изменений
            }
        }
        current = std::move(next);
        memo[i + 1] = current; // Сохраняем результат в кэш
    }
    return current;
}

BoundingBox computeBoundingBox(const std::vector<char>& instructions, float angle, float length) {
    sf::Vector2f position{0, 0}; // Начальная позиция (потом скорректируем)
    float current_angle = 0;
    std::vector<std::pair<sf::Vector2f, float>> stack;

    sf::Vector2f min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    sf::Vector2f max = {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};

    for (char c : instructions) {
        if (c == 'F') {
            sf::Vector2f newPosition = position + sf::Vector2f(std::cos(current_angle * 3.14159f / 180) * length,
                                                               std::sin(current_angle * 3.14159f / 180) * length);
            position = newPosition;
        } else if (c == '+') {
            current_angle += angle;
        } else if (c == '-') {
            current_angle -= angle;
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

sf::Vector2f calculateStartPosition(sf::RenderTexture& texture, const std::vector<char>& instructions, BoundingBox fractal_size, float angle, float length) {
    BoundingBox bbox = computeBoundingBox(instructions, angle, length);
    sf::Vector2f size = bbox.max - bbox.min;
    sf::Vector2f imageSize(texture.getSize().x, texture.getSize().y); // Размер изображения

    // float scale = std::min(imageSize.x / size.x, imageSize.y / size.y) * 0.9f; // 0.9 для отступов // Вычисляем масштаб, чтобы фрактал вписался в окно

    float scale = std::min(imageSize.x / size.x, imageSize.y / size.y); // без отступов
    sf::Vector2f start_position = (imageSize - (size * scale)) * 0.5f - bbox.min * scale; // Корректируем начальную позицию (центрируем)

    return start_position;
}

void drawLSystem(sf::RenderTexture& texture, std::vector<char>& instructions, float angle, float length, sf::Vector2f start_position, float start_angle) {

    std::vector<std::pair<sf::Vector2f, float>> stack;
    sf::VertexArray lines(sf::PrimitiveType::Lines);

    for (char c : instructions) {
        if (c == 'F') {
            sf::Vector2f newPosition = start_position + sf::Vector2f(cos(start_angle * 3.14159 / 180) * length, sin(start_angle * 3.14159 / 180) * length);
            lines.append(sf::Vertex({start_position, sf::Color::Black}));
            lines.append(sf::Vertex({newPosition, sf::Color::Black}));
            start_position = newPosition;
        } else if (c == '+') {
            start_angle += angle;
        } else if (c == '-') {
            start_angle -= angle;
        } else if (c == '[') {
            stack.emplace_back(start_position, start_angle);
        } else if (c == ']') {
            auto state = stack.back();
            stack.pop_back();
            start_position = state.first;
            start_angle = state.second;
        }
    }

    texture.clear(sf::Color::White);
    texture.draw(lines);
    texture.display();
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

// Функция загрузки BMP и бинаризации
std::vector<std::vector<bool>> readBmp(const std::string& filename, int& width, int& height) {
    std::ifstream file(filename, std::ios::binary);
    if(!file) {
        std::cerr<< "Error open " << filename << ".bmp file" << std::endl;
        return {};
    }

    // Читаем заголовок BMP
    unsigned char header[54];
    file.read(reinterpret_cast<char*>(header), 54);

    // Размеры изображения
    width = *(int*)&header[18];
    height = *(int*)&header[22];

    // BMP использует 4-байтовое выравнивание строк
    int row_padded = width * 4;
    std::vector<unsigned char> data(row_padded * height);
    file.read(reinterpret_cast<char*>(data.data()), data.size());

    std::vector<std::vector<bool>> binaryImage(height, std::vector<bool>(width, false));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (height - y - 1) * row_padded + x * 4;
            if (index + 3 >= data.size()) {
                std::cerr << "Error: out of array in readBmp function " << std::endl;
                return {};
            }

            unsigned char r = data[index];
            binaryImage[y][x] = (r < 128);
        }
    }

    return binaryImage;
}

// int countBoxes(const std::vector<std::vector<bool>>& image, int boxSize) {
//     int height = image.size();
//     int width = image[0].size();
//     int count = 0;

//     for (int y = 0; y < height; y += boxSize) {
//         for (int x = 0; x < width; x += boxSize) {
//             bool hasFractal = false;
//             for (int dy = 0; dy < boxSize && (y + dy) < height; ++dy) {
//                 for (int dx = 0; dx < boxSize && (x + dx) < width; ++dx) {
//                     if (image[y + dy][x + dx]) {
//                         hasFractal = true;
//                         break;
//                     }
//                 }
//                 if (hasFractal) break;
//             }
//             if (hasFractal) count++;
//         }
//     }

//     return count;
// }

// double calculateFractalDimension(const std::vector<std::vector<bool>>& image) {
//     std::vector<int> boxSizes = {2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64}; // Больше значений
//     std::vector<double> logN, logE;

//     for (int boxSize : boxSizes) {
//         int N = countBoxes(image, boxSize);
//         if (N == 0) continue; // Исключаем возможные ошибки логарифма

//         logN.push_back(std::log(N));
//         logE.push_back(std::log(1.0 / boxSize));
//     }

//     double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
//     int n = logN.size();
//     for (int i = 0; i < n; i++) {
//         sumX += logE[i];
//         sumY += logN[i];
//         sumXY += logE[i] * logN[i];
//         sumXX += logE[i] * logE[i];
//     }

//     double meanX = sumX / n;
//     double meanY = sumY / n;

//     double numerator = 0, denominator = 0;
//     for (int i = 0; i < n; i++) {
//         numerator   += (logE[i] - meanX) * (logN[i] - meanY);
//         denominator += (logE[i] - meanX) * (logE[i] - meanX);
//     }

//     return numerator / denominator;
// }

double processArrays(const std::vector<int>& ns, const std::vector<int>& countResults) {
    std::vector<double> logN, logE;

    for (size_t i = 0; i < ns.size(); i++) {
        if (countResults[i] == 0) continue; // добавляем 1e-10 вместо пропуска
        // logN.push_back(std::log(countResults[i])); // before
        logN.push_back(std::log(countResults[i] + 1e-10)); // after
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

double calculateFractalDimension(const std::vector<std::vector<bool>>& image) {
    int width = image[0].size();
    int height = image.size();
    int minL = std::min(width, height);
    int maxN = static_cast<int>(std::floor(minL * 0.2)); //before = 0.2

    std::vector<int> ns(10); // before: 10 result = 1,2575 , after: 20
    std::vector<int> countResults(10);

    for (int i = 0, n = maxN; i < 10; i++) {
        ns[i] = n;
        n = static_cast<int>(std::floor(n / 1.3)); // before: 1.3
    }

    for (int i = 0; i < 10; i++) {
        countResults[i] = countBoxes(image, ns[i]);
    }

    return processArrays(ns, countResults);
}

#endif
