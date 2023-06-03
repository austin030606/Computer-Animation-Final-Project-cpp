#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "simulation.h"
#define WINDOW_SIZE 1500

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "My window");
    bool holding = false, update = true, vac = false;
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
                // fluid.addVelocityX(row, col, 1000);
                // fluid.addVelocityY(row, col, 1000);
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                holding = false;
            }
            if (event.type == sf::Event::MouseMoved && update) {
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                int row = (int)(pos.x / cellSize), col = (int)(pos.y / cellSize);
                // std::cout << row << ' ' << col << '\n';
                if (row >= N + 2) row = N + 1;
                if (col >= N + 2) col = N + 1;
                fluid.addVelocityX(row, col, 1000 * (pos.x - x_prev));
                fluid.addVelocityY(row, col, 1000 * (pos.y - y_prev));
                x_prev = pos.x;
                y_prev = pos.y;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    fluid.addSqrBnd();
                    std::cout << "the space key was pressed" << std::endl;
                }
                if (event.key.code == sf::Keyboard::B) {
                    fluid.addBckBnd();
                    std::cout << "the space key was pressed" << std::endl;
                }
                if (event.key.code == sf::Keyboard::S) {
                    update = !update;
                    std::cout << "the s key was pressed" << std::endl;
                }
                if (event.key.code == sf::Keyboard::V) {
                    vac = !vac;
                    std::cout << "the v key was pressed" << std::endl;
                }
                if (event.key.code == sf::Keyboard::T){
                    for (int i = 0; i < N -10; i++) {
                        for (int j = 0; j < N -10; j++) {
                            fluid.setVelocityX(i, j, 1000);
                        }
                    }
                }
            }
        }

        if (vac) {
            for (int i = 0; i < 10; i++) {
                fluid.setVelocityX(i, i, 10000);
                fluid.setVelocityY(i, i, 10000);
                fluid.addDensity(i, i, 500);
            }

        }

        // for (int i = 0; i < N + 2; i++) {
        //     for (int j = 0; j < N + 2; j++) {
        //         if ((i == N / 2 - 1|| i == N / 2) && j < 10) {
        //             fluid.setVelocityY(i, j, 10000);
        //             fluid.addDensity(i, j, 500);
        //         }
        //         // if (i == N / 2 && j > N - 10) {
        //         //     fluid.setVelocityY(i, j, -100000);
        //         //     fluid.addDensity(i, j, 500);
        //         // }
        //     }
        // }
        if (update) fluid.Update();
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