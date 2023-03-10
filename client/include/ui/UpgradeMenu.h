//
// Created by cedric on 1/8/23.
//

#pragma once

#include <vector>
#include "SFML/Graphics/Drawable.hpp"
#include "UpgradeManager.h"
#include "SFML/Graphics/Texture.hpp"
#include "world/Space.h"

class UpgradeMenu : public sf::Drawable {
protected:
    mutable sf::Sprite locked;
    mutable sf::Sprite selected;
    Space& space;
    mutable size_t indexSelected = 0;

    std::vector<std::vector<std::vector<Upgrade>>> upgradeTree{
            {{BOOST_BASIC}, {BOOST_ULTRA}},
            {{LASER_DOUBLE}, {LASER_TRIANGLE}, {LASER_SHOTGUN, LASER_CRAZY}},
            {{NUKE_SIMPLE}},
            {{FLAMETHROWER_SIMPLE}},
    };

    mutable std::vector<Upgrade> displayedItems{};
    std::unordered_map<Upgrade, sf::Sprite*> upgrade_sprites;
    UpgradeManager& upgradeManager;
    bool upgradeBlockComplete(const std::vector<Upgrade>& upgradeBlock) const;
public:
    explicit UpgradeMenu(Space& space, UpgradeManager &upgradeManager);

    void moveLeft();
    void moveRight();
    Upgrade select();
    void handleMouse(sf::Vector2i pos);
    void draw(sf::RenderTarget &target, const sf::RenderStates &states) const override;
};
