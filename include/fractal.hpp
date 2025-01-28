#ifndef Fractal_hpp
#define Fractal_hpp

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <cmath>

struct BoundingBox {
    sf::Vector2f min, max;
};

std::vector<char> generateLSystem(int iterations, const std::vector<char>& axiom, const std::unordered_map<char, std::vector<char>>& rules) {
    static std::unordered_map<int, std::vector<char>> memo; // Кэш для мемоизации

    if (memo.count(iterations)) return memo[iterations]; // Используем кэш

    std::vector<char> current = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::vector<char> next;
        for (char c : current) {
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
    float scale = std::min(imageSize.x / size.x, imageSize.y / size.y) * 0.9f; // 0.9 для отступов // Вычисляем масштаб, чтобы фрактал вписался в окно
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

#endif
