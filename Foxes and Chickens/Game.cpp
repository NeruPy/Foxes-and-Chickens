#include "Game.h"
#include <iostream>

Game::Game(int rows, int cols, int cellSize)
    : rows(rows), cols(cols), cellSize(cellSize),
    window(sf::VideoMode(cols* cellSize, rows* cellSize), "Foxes and Chickens Game") {
    loadTextures();
    grid = new Grid(rows, cols, cellSize);  // �������������� ����� ����� �������� ��������

    // ��������� �������� ��� �����
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (i < 3 && j < 3) {
                grid->setTexture(i, j, &topTexture);  // ������� ������ 3 �� 3
            }
            else {
                grid->setTexture(i, j, &mainTexture);  // ��������� ������
            }
        }
    }
}

Game::~Game() {
    delete grid;  // ����������� ������, ���������� ��� �����
}

void Game::loadTextures() {
    if (!mainTexture.loadFromFile("textures/grass-texture1.png")) {  // ��������� ���� ������������ ������������ �����
        std::cerr << "Error loading main texture" << std::endl;
    }
    if (!topTexture.loadFromFile("textures/grass-texture2.png")) {  // ��������� ���� ������������ ������������ �����
        std::cerr << "Error loading top texture" << std::endl;
    }
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void Game::update() {
    // ���������� ������ ���� (���� ����������)
}

void Game::render() {
    window.clear(sf::Color::White);

    // ���������� �������� � ������� ������ ����� ���������������
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // ������������� ������, ���� ����������
            grid->draw(window);
        }
    }

    window.display();
}
