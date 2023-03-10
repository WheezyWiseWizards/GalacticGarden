//
// Created by william on 06/01/23.
//

#ifndef LD52_CLIENT_SPACESCREEN_H
#define LD52_CLIENT_SPACESCREEN_H

#include "GameAssets.h"
#include "WIZ/game/Screen.h"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Audio/Music.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "world/Space.h"

#include "WIZ/input/Mapping.h"
#include "new_wiz/NewMappingDatabase.h"
#include "ui/GameOverMenu.h"
#include "MiniMap.h"

#include "WIZ/ui/DialogBox.h"
#include "ui/PictureDialogBox.h"
#include "ui/Selection.h"
#include "ui/UpgradeMenu.h"

class SpaceScreen : public wiz::Screen, public wiz::WindowListener, public wiz::InputListener {
    std::string name = "SpaceScreen";
    sf::Sound sound;
    sf::Music* music;

    sf::Text shopText;
    bool shopIsOpen = false;

    sf::Sprite background;
    Space space;
    MiniMap miniMap;

    NewMappingDatabase mappingDatabase;
    wiz::Mapping mapping;
    bool mappingFound = false;
    std::map<int, sf::Keyboard::Key> mapJoyButtonToKey;
    std::map<sf::Joystick::Axis, std::pair<sf::Keyboard::Key, sf::Keyboard::Key>> mapJoyAxisToKey;
    
    sf::Vector2f smoothPosition, cameraPosition, shipSmoothVelocity;
    float zoom = 1.0f;
    
    sf::Sprite energySprite;
    sf::Text energyText;
    
    GameOverMenu gameOverMenu;
    float gameoverCooldown = 0.5f;

    const float SPACE_STATION_STORE_DIS_SQ = 50.0f;

    PictureDialogBox dialogBox;

    Selection weaponSelectionUi;
    Selection seedSelectionUi;
    Selection boostSelectionUi;
    UpgradeMenu upgradeMenu;

    mutable sf::Sprite dim;
    sf::Sprite infographicSprite;
    sf::Text infographicText;

    float intervalBetweenKeyPresses = 150.0f;
    float timeBetweenKeyPresses = .0f;
    bool canProcessKeyPress = true;

public:
    constexpr const static sf::Vector2f UI_VIEW_SIZE = { 1920.0f, 1080.0f };
    
    SpaceScreen(wiz::Game& game);

    void initMapJoyButtonToKey();

    void tick(float delta) override;

    void render(sf::RenderTarget& target) override;

    void show() override;

    void hide() override;

    const std::string& getName() const override;

    void windowClosed() override;

    void mouseButtonPressed(const sf::Event::MouseButtonEvent &mouseButtonEvent) override;

    void keyPressed(const sf::Event::KeyEvent &keyEvent) override;

    void processInput(float delta);

    void mouseWheelScrolled(const sf::Event::MouseWheelScrollEvent& mouseWheelScrollEvent) override;

    const Space& getSpace() const;

    bool isShopIsOpen() const;

    void setShopIsOpen(bool shopIsOpen);

    void mouseMoved(const sf::Event::MouseMoveEvent &mouseMoveEvent) override;

    void joystickButtonPressed(const sf::Event::JoystickButtonEvent& joystickButtonEvent) override;

    void joystickMoved(const sf::Event::JoystickMoveEvent& joystickMoveEvent) override;
};

#endif //LD52_CLIENT_SPACESCREEN_H
