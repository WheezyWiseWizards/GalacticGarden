//
// Created by william on 08/01/23.
//

#ifndef LD52_CLIENT_SELECTIONSCROLL_H
#define LD52_CLIENT_SELECTIONSCROLL_H

#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Text.hpp"
#include "WIZ/asset/TextureAsset.h"
#include "GameAssets.h"
#include "WIZ/asset/AssetLoader.h"

class SpaceScreen;

enum SelectionType {
    WEAPON,
    SEED,
    BOOSTER
};

class WeaponTextureGetter {
    std::vector<sf::Texture*> weaponTextures;

public:
    WeaponTextureGetter(wiz::AssetLoader& assets) {
        weaponTextures =
                {assets.get(GameAssets::TEXTURE_SMALL_LAZER),
                 assets.get(GameAssets::TEXTURE_LASER_DOUBLE),
                 assets.get(GameAssets::TEXTURE_LASER_TRIANGLE),
                 assets.get(GameAssets::TEXTURE_LASER_FOUR_WAY),
                 assets.get(GameAssets::TEXTURE_LASER_CRAZY),
                 assets.get(GameAssets::TEXTURE_NUKE_SIMPLE)};
    }

    ~WeaponTextureGetter() = default;

    std::vector<sf::Texture *> get() {
        return weaponTextures;
    }
};

class SelectionScroll : public sf::Drawable {
    mutable std::vector<sf::Sprite> backdrops;
    mutable std::vector<sf::Sprite> items;
    mutable sf::Sprite selectHighLight;
    SelectionType type;
    int selection = 1;
    bool enableScroll = false;

    float openTime = 2000.f;
    float openDuration = .0f;

    float xOffsetSpacingBetweenBoxes = 50.f;
    float yOffset = -60.f;
    sf::Vector2f selectionDisPos;

    float changeSelectionInterval = 50.f;
    float timeBetweenChange = .0f;

public:
    explicit SelectionScroll(SpaceScreen& screen, SelectionType type, int numberOfItems, sf::Vector2f selectionDisPos);

    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    bool isEnableScroll() const;

    void setEnableScroll(bool enableScroll);

    int getSelection() const;

    void setSelection(int selection);

    void changeSelection(bool changeToNext);

    void update(float delta);
};

#endif //LD52_CLIENT_SELECTIONSCROLL_H
