//
// Created by Alexander Winter on 2023-01-07.
//

#include "ui/GameOverMenu.h"
#include "SpaceScreen.h"

#include "util/MathUtil.h"


GameOverMenu::GameOverMenu(SpaceScreen& screen) {
    dim.setTexture(*screen.getAssets().get(GameAssets::WHITE_PIXEL));
    dim.setPosition({ 0.0f, 0.0f });
    dim.setScale(SpaceScreen::UI_VIEW_SIZE);
    dim.setColor(sf::Color(0, 0, 0, 128));
    
    gameover.setTexture(*screen.getAssets().get(GameAssets::GAMEOVER));
    sf::Vector2f vec = SpaceScreen::UI_VIEW_SIZE.cwiseDiv(MathUtil::toFloat(gameover.getTexture()->getSize()));
    gameover.setPosition({ 0.0f, 0.0f });
    gameover.setScale({ vec.x, vec.y / 4.0f * 16.0f / 9.0f });

    scoreText.setString("");
    scoreText.setScale({ 1.0f, 1.0f });
    scoreText.setFont(*screen.getAssets().get(GameAssets::VT323_TTF));
    scoreText.setCharacterSize(60);
    scoreText.setFillColor(sf::Color::White);
    sf::FloatRect textRect2 = scoreText.getLocalBounds();
    scoreText.setPosition({ SpaceScreen::UI_VIEW_SIZE.x / 2.0f - textRect2.width / 2.0f, 500.0f});

    text.setString("The Cosmic Crows have defeated you and are now eating your vegetables.\n\nPress any key to restart");
    text.setScale({1.0f, 1.0f});
    text.setFont(*screen.getAssets().get(GameAssets::VT323_TTF));
    text.setCharacterSize(60);
    text.setFillColor(sf::Color::White);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setPosition({ SpaceScreen::UI_VIEW_SIZE.x / 2.0f - textRect.width / 2.0f, 700.0f});
}

void GameOverMenu::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
    target.draw(dim);
    target.draw(gameover);
    target.draw(scoreText);
    target.draw(text);
}

void GameOverMenu::setStats(int waveNumber, int score) {
    scoreText.setString("Wave: " + std::to_string(waveNumber) + "\nScore: " + std::to_string(score));
    sf::FloatRect textRect2 = scoreText.getLocalBounds();
    scoreText.setPosition({ SpaceScreen::UI_VIEW_SIZE.x / 2.0f - textRect2.width / 2.0f, 500.0f});
}
