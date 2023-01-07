//
// Created by cedric on 1/6/23.
//

#pragma once

#include "Entity.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Sprite.hpp"

class Ship : public Entity {
protected:
	mutable sf::Sprite sprite;
    float fire_delay = 0.4f;
    float time_since_last_fire = 0.f;
    float rotation = 0.0f;
    float maxSpeed = 4.f; // 4 units per second
    float acc = 10.f; // 10 units per second ^ 2
    sf::Vector2f moveVelocity = {0.f, 0.f};
    bool rotateLeft = false, rotateRight = false;

public:
	Ship(Space& space, const sf::Vector2f& location);

    float getZOrder() const override;

	void tick(float delta) override;
	
	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    void fire();

    void moveInDirOfVec(const sf::Vector2f& moveVec, float good_delta);

    void setRotation(float rotationRad);

    float getRotation() const;
};
