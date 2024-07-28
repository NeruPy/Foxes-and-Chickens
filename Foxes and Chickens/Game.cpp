#include "Game.h"
#include <iostream>

Game::Game(int rows, int cols, int cellSize)
    : rows(rows), cols(cols), cellSize(cellSize),
    window(sf::VideoMode(cols* cellSize, rows* cellSize), "Foxes and Chickens Game"),
    selectedChicken(nullptr), selectedFox(nullptr) {
    loadTextures();
    grid = new Grid(rows, cols, cellSize);  // �������������� ����� ����� �������� �������

    // ��������� ������� ��� �����
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

    // ������������� �����
    for (int i = 0; i < 20; ++i) {
        chickens.push_back(Chicken(i % cols * cellSize + cellSize / 2, i / cols * cellSize + cellSize / 2, &chickenTexture));
    }

    // ������������� �����
    for (int i = 0; i < 2; ++i) {
        foxes.push_back(Fox((cols - 1 - i) * cellSize + cellSize / 2, (rows - 1) * cellSize + cellSize / 2, &foxTexture));
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
    if (!chickenTexture.loadFromFile("textures/chicken.png")) {  // ��������� ���� ������������ ������������ �����
        std::cerr << "Error loading chicken texture" << std::endl;
    }
    if (!foxTexture.loadFromFile("textures/fox3.png")) {  // ��������� ���� ������������ ������������ �����
        std::cerr << "Error loading fox texture" << std::endl;
    }
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        processEvents();
        float deltaTime = clock.restart().asSeconds();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            handleMouseClick(event.mouseButton.x, event.mouseButton.y);
        }
    }
}

void Game::update(float deltaTime) {
    for (auto& chicken : chickens) {
        chicken.update(deltaTime);
    }

    for (auto& fox : foxes) {
        fox.update(deltaTime);
    }
}

void Game::render() {
    window.clear(sf::Color::White);
    grid->draw(window);

    // ���������� ��������� ����
    for (const auto& highlight : possibleMoveHighlights) {
        window.draw(highlight);
    }

    for (const auto& chicken : chickens) {
        chicken.draw(window);
    }

    for (const auto& fox : foxes) {
        fox.draw(window);
    }

    window.display();
}

void Game::handleMouseClick(int x, int y) {
    sf::Vector2f mousePos(static_cast<float>(x), static_cast<float>(y));

    // ���� ������� ������, ��������� ��������� ����
    if (selectedChicken) {
        for (const auto& highlight : possibleMoveHighlights) {
            if (highlight.getGlobalBounds().contains(mousePos)) {
                selectedChicken->startMoving(highlight.getPosition() + sf::Vector2f(cellSize / 2, cellSize / 2));
                selectedChicken = nullptr;
                possibleMoveHighlights.clear();
                return;
            }
        }
        selectedChicken = nullptr;
        possibleMoveHighlights.clear();
    } else if (selectedFox) {
        for (const auto& highlight : possibleMoveHighlights) {
            if (highlight.getGlobalBounds().contains(mousePos)) {
                // ���������� ���������� � �������� ������
                sf::Vector2f dest = highlight.getPosition() + sf::Vector2f(cellSize / 2, cellSize / 2);
                dest.x = static_cast<int>(dest.x / cellSize) * cellSize + cellSize / 2;
                dest.y = static_cast<int>(dest.y / cellSize) * cellSize + cellSize / 2;
                selectedFox->startMoving(dest);
                selectedFox = nullptr;
                possibleMoveHighlights.clear();
                return;
            }
        }
        selectedFox = nullptr;
        possibleMoveHighlights.clear();
    } else {
        for (auto& chicken : chickens) {
            if (chicken.getBounds().contains(mousePos)) {
                selectedChicken = &chicken;
                possibleMoveHighlights.clear();

                // ��������� ��������� ���� (�����, ������, ����)
                sf::Vector2f pos = chicken.getPosition();
                sf::Vector2f moves[] = {
                    sf::Vector2f(pos.x - cellSize, pos.y), // �����
                    sf::Vector2f(pos.x + cellSize, pos.y), // ������
                    sf::Vector2f(pos.x, pos.y + cellSize)  // ����
                };
                for (const auto& move : moves) {
                    if (move.x >= 0 && move.x < cols * cellSize && move.y < rows * cellSize && !isCellOccupied(move)) {
                        sf::RectangleShape highlight(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
                        highlight.setFillColor(sf::Color(0, 255, 0, 128));
                        highlight.setPosition(move.x - cellSize / 2, move.y - cellSize / 2);
                        possibleMoveHighlights.push_back(highlight);
                    }
                }
                return;
            }
        }

        // ���������, �������� �� �� �� ����
        for (auto& fox : foxes) {
            if (fox.getBounds().contains(mousePos)) {
                selectedFox = &fox;
                possibleMoveHighlights.clear();

                // ��������� ��������� ���� (�����, ������, ����, �����)
                sf::Vector2f pos = fox.getPosition();
                sf::Vector2f moves[] = {
                    sf::Vector2f(pos.x - cellSize, pos.y), // �����
                    sf::Vector2f(pos.x + cellSize, pos.y), // ������
                    sf::Vector2f(pos.x, pos.y + cellSize), // ����
                    sf::Vector2f(pos.x, pos.y - cellSize)  // �����
                };
                for (const auto& move : moves) {
                    if (move.x >= 0 && move.x < cols * cellSize && move.y >= 0 && move.y < rows * cellSize && !isCellOccupied(move)) {
                        sf::RectangleShape highlight(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
                        highlight.setFillColor(sf::Color(0, 255, 0, 128)); // ������� ���������
                        highlight.setPosition(move.x - cellSize / 2, move.y - cellSize / 2);
                        possibleMoveHighlights.push_back(highlight);
                    }
                }
                return;
            }
        }
    }
}


bool Game::isCellOccupied(const sf::Vector2f& position) const {
    for (const auto& chicken : chickens) {
        if (chicken.getBounds().contains(position)) {
            return true;
        }
    }
    for (const auto& fox : foxes) {
        if (fox.getBounds().contains(position)) {
            return true;
        }
    }
    return false;
}
