//
// Created by cedric on 1/8/23.
//

#include "ui/UpgradeMenu.h"
#include "world/Space.h"
#include "GameAssets.h"

UpgradeMenu::UpgradeMenu(Space& space, UpgradeManager &upgradeManager) : space(space), upgradeManager(upgradeManager) {

    wiz::AssetLoader& ass = space.getAssets();
    upgrade_sprites[Upgrade::LASER_DOUBLE] = new sf::Sprite{*ass.get(GameAssets::TEXTURE_LASER_DOUBLE)};
    upgrade_sprites[Upgrade::LASER_TRIANGLE] = new sf::Sprite{*ass.get(GameAssets::TEXTURE_LASER_TRIANGLE)};
    upgrade_sprites[Upgrade::LASER_SHOTGUN] = new sf::Sprite{*ass.get(GameAssets::TEXTURE_LASER_FOUR_WAY)};
    upgrade_sprites[Upgrade::LASER_CRAZY] = new sf::Sprite{*ass.get(GameAssets::TEXTURE_LASER_CRAZY)};
    upgrade_sprites[Upgrade::NUKE_SIMPLE] = new sf::Sprite{*ass.get(GameAssets::TEXTURE_NUKE_SIMPLE)};
    upgrade_sprites[Upgrade::FLAMETHROWER_SIMPLE] = new sf::Sprite{*ass.get(GameAssets::TEXTURE_FLAMETHROWER_SIMPLE)};
    upgrade_sprites[Upgrade::BOOST_BASIC] = new sf::Sprite{*ass.get(GameAssets::TEXTURE_BOOST_BASIC)};
    upgrade_sprites[Upgrade::BOOST_ULTRA] = new sf::Sprite{*ass.get(GameAssets::TEXTURE_BOOST_ULTRA)};

    locked.setTexture(*ass.get(GameAssets::TEXTURE_LOCKED));
    selected.setTexture(*ass.get(GameAssets::TEXTURE_SELECTED));
}

void UpgradeMenu::draw(sf::RenderTarget &target, const sf::RenderStates &states) const {
    // dont update without updating handleMouse
    float offsetX = 0.0f;
    float xPos = 200.0f;
    float yPos = 400.0f;
    // end of danger

    displayedItems.clear();

    for(const auto& upgradeLine : upgradeTree)
    {
        size_t index = -1;
        for(size_t i = 0; i < upgradeLine.size(); i++)
        {
            bool upgradeBlockDone = upgradeBlockComplete(upgradeLine[i]);
            if(!upgradeBlockDone)
            {
                index = i;
                break;
            }
        }

        if(index != -1)
        {
            const std::vector<Upgrade>& upgradeBlock = upgradeLine.at(index);
            for(size_t i = 0; i < upgradeBlock.size(); i++)
            {
                if(!upgradeManager.has_unlocked(upgradeBlock[i]))
                    displayedItems.push_back(upgradeBlock[i]);
            }
        }
    }

    std::sort(displayedItems.begin(), displayedItems.end(), [&](const Upgrade& a, const Upgrade& b) {
        return upgradeManager.get_cost(a) < upgradeManager.get_cost(b);
    });

    if(indexSelected >= displayedItems.size())
    {
        indexSelected = 0;
    }

    for(size_t i = 0; i < displayedItems.size(); i++)
    {
        const Upgrade& upgrade = displayedItems[i];

        if(indexSelected == i)
        {
            selected.setPosition({xPos + offsetX - 8.0f, yPos - 8.0f});
            selected.setScale({8.0f, 8.0f});
            target.draw(selected);
        }
        sf::Sprite* sprite = upgrade_sprites.at(upgrade);
        sprite->setPosition({xPos + offsetX, yPos});
        sprite->setScale({8.0f, 8.0f}); // will make alex cringe
        target.draw(*sprite);
        locked.setPosition(sprite->getPosition());
        locked.setScale(sprite->getScale());
        if(upgradeManager.get_cost(upgrade) > space.getShip().getEnergy())
        {
            target.draw(locked);
        }

        sf::Text price;
        int price_value = upgradeManager.get_cost(upgrade);
        price.setString(std::to_string(price_value));
        price.setPosition({xPos + offsetX + 20, yPos + 140.0f});
        price.setCharacterSize(96);
        price.setFont(*space.getAssets().get(GameAssets::VT323_TTF));
        target.draw(price);
        offsetX += 300;
    }
}

void UpgradeMenu::handleMouse(sf::Vector2i pos) {

    // kwik impl
    float xPos = 200.0f;
    float xOffset = 300.f;
    float xMax = xPos + xOffset * displayedItems.size();
    // garbage that will trigger winter
    float ratio = (pos.x - xPos)/(xMax - xPos);
    indexSelected = floor(ratio * displayedItems.size());
    if(indexSelected >= displayedItems.size())
        indexSelected = displayedItems.size() - 1;
}

bool UpgradeMenu::upgradeBlockComplete(const std::vector<Upgrade> &upgradeBlock) const {
    for(const auto &upgrade: upgradeBlock)
    {
        if(!upgradeManager.has_unlocked(upgrade))
            return false;
    }

    return true;
}

void UpgradeMenu::moveLeft() {
    if(this->indexSelected >= 0)
    {
        this->indexSelected--;
    }
}

void UpgradeMenu::moveRight() {
    if(this->indexSelected < displayedItems.size() - 1)
    {
        this->indexSelected++;
    }
}

Upgrade UpgradeMenu::select() {
    return displayedItems[indexSelected];
}

