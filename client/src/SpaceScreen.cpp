//
// Created by william on 06/01/23.
//

#include "SpaceScreen.h"
#include "SFML/Window/Touch.hpp"
#include <string>
#include "GameAssets.h"

SpaceScreen::SpaceScreen(wiz::Game& game)
	: Screen(game), space(game.getAssets()), mappingDatabase(), gameOverMenu(*this) {
    mappingDatabase.loadFromCSV(*getGame().getAssets().get(GameAssets::CONTROLLER_DB));
    cameraPosition = space.getShip().getLocation();
    energySprite.setTexture(*space.getAssets().get(GameAssets::TEXTURE_ENERGY));
}

void SpaceScreen::tick(float delta) {
    if(!gameover) {
        processInput(delta);
        space.tick(delta);   
    } else {
        gameoverCooldown -= delta / 1000.0f;
    }
}

void SpaceScreen::render(sf::RenderTarget& target) {

    sf::Vector2f vec = { 1.0f, 1.0f };
    vec.x /= static_cast<float>(background.getTextureRect().getSize().x);
    vec.y /= static_cast<float>(background.getTextureRect().getSize().y);
    background.setScale(vec);
    energySprite.setPosition({50.f, 900.f});
    energySprite.setScale({4.f, 4.f});

    energyText.setString(std::to_string(space.getShip().getEnergy()));
    energyText.setPosition({ 175.f, 925.f});
    energyText.setScale({2.f, 2.f});
    energyText.setFillColor(sf::Color::White);
    energyText.setFont(*space.getAssets().get(GameAssets::SANS_TTF));
    
	target.clear();
	target.setView(sf::View({ 0.5f, 0.5f }, { 1.0f, 1.0f }));
	target.draw(background);
    
    cameraPosition = cameraPosition * 0.995f + space.getShip().getLocation() * 0.005f;
	target.setView(sf::View(cameraPosition, Space::VIEW_SIZE));
	target.draw(space);

    // ui
    target.setView(sf::View(SpaceScreen::UI_VIEW_SIZE / 2.0f, SpaceScreen::UI_VIEW_SIZE));
    target.draw(energySprite);
    target.draw(energyText);
    if(gameover)
        target.draw(gameOverMenu);
}

void SpaceScreen::show() {
	background.setTexture(*getGame().getAssets().get(GameAssets::BACKGROUND));

	getGame().addWindowListener(this);
    getGame().addInputListener(this);
}

void SpaceScreen::hide() {
	getGame().removeWindowListener(this);
    getGame().removeInputListener(this);
}

void SpaceScreen::mouseButtonPressed(const sf::Event::MouseButtonEvent &mouseButtonEvent) {
    if(gameover && gameoverCooldown <= 0.0f) {
        getGame().setScreen(new SpaceScreen(getGame()));
        return;
    }
}

void SpaceScreen::keyPressed(const sf::Event::KeyEvent &keyEvent) {
    if(gameover && gameoverCooldown <= 0.0f) {
        getGame().setScreen(new SpaceScreen(getGame()));
        return;
    }
    
    switch(keyEvent.code) {
        case sf::Keyboard::Escape:
            if(!gameover)
                gameover = true;
            break;
        
        case sf::Keyboard::Num1:
        case sf::Keyboard::Numpad1:
            space.getShip().setLazerType(LazerType::SIMPLE);
            break;
        
        case sf::Keyboard::Num2:
        case sf::Keyboard::Numpad2:
            space.getShip().setLazerType(LazerType::DOUBLE);
            break;
        
        case sf::Keyboard::Num3:
        case sf::Keyboard::Numpad3:
            space.getShip().setLazerType(LazerType::TRIANGLE);
            break;
        
        case sf::Keyboard::Num4:
        case sf::Keyboard::Numpad4:
            space.getShip().setLazerType(LazerType::FOUR_WAY);
            break;
            
        case sf::Keyboard::Num5:
        case sf::Keyboard::Numpad5:
            space.getShip().setLazerType(LazerType::CRAZY);
            break;
            
        default:
            break;
    }
}

const std::string& SpaceScreen::getName() const {
	return name;
}

void SpaceScreen::windowClosed() {
	getGame().getWindow().close();
}

void SpaceScreen::processInput(float delta) {
    bool connected = sf::Joystick::isConnected(0);
    // TODO: figure out why TF fetching the joystick name doesn't work
    mappingFound = connected;
//    if (connected) {
//        sf::Joystick::Identification id = sf::Joystick::getIdentification(0);
//        if (controllerDisconnected && mappingDatabase.hasMapping(id.name)) {
//            mapping = mappingDatabase.getMapping(id.name);
//            controllerDisconnected = false;
//            mappingFound = true;
//        }
//    } else {
//        controllerDisconnected = true;
//        mappingFound = false;
//    }

    bool isFiring = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    bool isBoosting = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);

    if(isFiring)
        space.getShip().fire();

    space.getShip().setIsBoosting(isBoosting);

//    bool isPlanting = mappingFound && connected ? sf::Joystick::isButtonPressed(0, mapping.getButton(wiz::MapButton::Left_Shoulder)) :
//            (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
//            sf::Mouse::isButtonPressed(sf::Mouse::Button::Right));

    bool isPlanting = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    if (isPlanting)
        space.getShip().plantOnAsteroid(space);

    float xAxisInput = mappingFound && connected ? sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) :
                       (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
                       ? 100.00 : (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ? -100.00 : 0.00);

    float yAxisInput = mappingFound && connected ? sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) :
                       (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
                       ? -100.00 : (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ? 100.00 : 0.00);

    space.getShip().setIsIdle(std::abs(xAxisInput) <= 0.1 && std::abs(yAxisInput) <= 0.1f);
    sf::Vector2f moveVec = {xAxisInput, yAxisInput};

    space.getShip().moveInDirOfVec(moveVec, delta / 1000.f);

    sf::Vector2f pos = getWindow().mapPixelToCoords(sf::Mouse::getPosition(getWindow()), sf::View({ 0.f, 0.f }, Space::VIEW_SIZE));

    float rotation = 0.0f;
    if(pos.length() >= 0.0001)
    {
        pos = pos.normalized();
        rotation = pos.angle().asDegrees();
    }
    space.getShip().setRotation(rotation + 90.0f);
}
