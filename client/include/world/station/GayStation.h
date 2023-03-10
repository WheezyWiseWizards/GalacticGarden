//
// Created by william on 07/01/23.
//

#ifndef LD52_CLIENT_GAYSTATION_H
#define LD52_CLIENT_GAYSTATION_H

#include "SFML/Graphics/Sprite.hpp"
#include "world/Entity.h"
#include "world/weapon/Lazer.h"

class GayStation : public Entity {
protected:
    mutable sf::Sprite sprite, shieldSprite;

    float size = 10.0f;

    float bobbingDisplacement = 0.0f;
    bool bobbingDirection = true; // true: upwards, false: downwards
    float bob_starting_pos = 0.0f;
    float bob_speed = 0.3f;
    float bob_max_displacement = 0.1f;

    Faction fraction = FRIENDLY;

    sf::Shader* damageShader = nullptr;
    float redness = 0.0f;

public:
    explicit GayStation(Space& space, sf::Vector2f location);

    void tick(float delta) override;

    void draw(sf::RenderTarget &target, const sf::RenderStates &states) const override;

    [[nodiscard]]
    sf::Vector2f getVisualSize() const override {
        return sf::Vector2f { 15.0f, 15.0f };
    }

    bool shouldBeRemoved() const override;

    float getZOrder() const override;
};

#endif //LD52_CLIENT_GAYSTATION_H
