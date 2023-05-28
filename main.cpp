#include <iostream>
#include <SFML/Graphics.hpp>
#include "simulation.h"
#define WINDOW_SIZE 1500

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "My window");
    bool holding = false;
    Fluid fluid = Fluid();
    float tmp = N + 2;
    float cellSize = WINDOW_SIZE / tmp;

    while (window.isOpen()) {
        int x_prev, y_prev;
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                fluid.clear();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                holding = true;
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                x_prev = pos.x;
                y_prev = pos.y;
                int row = (int)(pos.x / cellSize), col = (int)(pos.y / cellSize);
                // std::cout << row << ' ' << col << '\n';
                if (row >= N + 2) row = N + 1;
                if (col >= N + 2) col = N + 1;
                fluid.addDensity(row, col, 10000);
                fluid.addVelocityX(row, col, 100000);
                fluid.addVelocityY(row, col, 100000);
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                holding = false;
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                int row = (int)(pos.x / cellSize), col = (int)(pos.y / cellSize);
                // std::cout << row << ' ' << col << '\n';
                if (row >= N + 2) row = N + 1;
                if (col >= N + 2) col = N + 1;
                fluid.addVelocityX(row, col, 100000 * (pos.x - x_prev));
                fluid.addVelocityY(row, col, 100000 * (pos.y - y_prev));
                x_prev = pos.x;
                y_prev = pos.y;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    std::cout << "the space key was pressed" << std::endl;
                }
            }
        }

        fluid.Update();
        window.clear();
        for (int i = 0; i < N + 2; i++) {
            for (int j = 0; j < N + 2; j++) {
                if (fluid.density[i][j] > 0) {
                    sf::RectangleShape box(sf::Vector2f(cellSize, cellSize));
                    box.setPosition(i * cellSize, j * cellSize);
                    int intensity = fluid.density[i][j] < 255 ? fluid.density[i][j] : 255;
                    box.setFillColor(sf::Color(intensity, intensity, intensity));
                    window.draw(box);
                }
            }
        }
        window.display();
    }

    return 0;
}