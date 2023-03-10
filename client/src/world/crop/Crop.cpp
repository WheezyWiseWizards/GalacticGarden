//
// Created by cedric on 1/6/23.
//

#include "world/crop/Crop.h"
#include "SFML/Graphics/Sprite.hpp"
#include "GameAssets.h"
#include "world/Space.h"
#include "util/SpriteUtil.h"
#include "world/FloatingText.h"

Crop::Crop(Asteroid& asteroid, 
           sf::Vector2f relLocation,
           sf::Texture* plantGrowing,
           sf::Texture* grown)
	: Entity(asteroid.getSpace(), 
             asteroid.getLocation() + relLocation.rotatedBy(sf::degrees(asteroid.getRotation()))), 
             asteroid(asteroid),
             relLocation(relLocation), 
             timeSincePlanted(0.0f), 
             health(0),
             plantGrowing(plantGrowing),
             grown(grown),
      progress(space.getAssets()){
    
    sprite.setTexture(*plantGrowing, true);
    SpriteUtil::setSpriteOrigin(sprite, { 0.5f, 1.0f });
    SpriteUtil::setSpriteSize(sprite, { 0.5f, 0.5f });
    
}

void Crop::tick(float delta) {
    if(shouldBeRemoved())
        return;
    
    if(harvested) {
        location += (space.getShip().getLocation() - location) * delta / 1000.0f * harvestingSpeed;
        
        if((space.getShip().getLocation() - location).lengthSq() < 0.1f) {
            dead = true;
            space.addEntity(new FloatingText(space, location, "+" + std::to_string((int)getEnergyGain()), sf::Color::Green, 1.5f));
            space.getShip().pickupSound.play();
        }
        return;
    }
    
    bool wasReady = isReady();
    
    timeSincePlanted += delta / 1000.0f;
    location = asteroid.getLocation() + relLocation.rotatedBy(sf::degrees(asteroid.getRotation()));

    if(isReady() && !wasReady)
        sprite.setTexture(*grown, true);
}

bool Crop::isReady() const {
    return timeSincePlanted >= getTimeToMaturity();
}

void Crop::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
    if(shouldBeRemoved())
        return;
    
    sprite.setPosition(location);
    progress.setPosition(location + sf::Vector2f { 0.0f, 0.1f });
    progress.setHealth(std::min(timeSincePlanted / getTimeToMaturity(), 1.0f));
    
    target.draw(sprite);
    if(!isReady())
        target.draw(progress);
}

void Crop::damage(float value) {
    if(shouldBeRemoved())
        return;
    
    this->health -= value;
    if(health <= 0) {
        asteroid.removeCrop(relLocation);
        dead = true;
    }
}

void Crop::harvest() {
    if(harvested)
        return;

    harvested = true;
    asteroid.removeCrop(relLocation);
}

bool Crop::shouldBeRemoved() const {
    return dead;
}

Asteroid& Crop::getAsteroid() const {
    return asteroid;
}

sf::Vector2f Crop::getLocationOnAsteroid() const {
    return relLocation;
}

bool Crop::isHarvested() {
    return harvested;
}
