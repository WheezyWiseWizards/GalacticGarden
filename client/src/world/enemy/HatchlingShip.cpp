//
// Created by cedric on 1/6/23.
//

#include "world/enemy/HatchlingShip.h"
#include "world/Space.h"
#include "GameAssets.h"
#include "world/crop/Crop.h"
#include "util/SpriteUtil.h"

HatchlingShip::HatchlingShip(Space &space, sf::Vector2f location) 
    : EnemyShip(space, location) {
    sprite.setTexture(*space.getAssets().get(GameAssets::TEXTURE_HATCHLING));
    sprite.setOrigin({ sprite.getTexture()->getSize().x / 2.0f, sprite.getTexture()->getSize().y / 2.0f });
    SpriteUtil::setSpriteSize(sprite, { 1.0f, 1.0f });
    targetAsteroid = nullptr;
    speed = 0.01f;
    damageAmount = 1;
    fire_delay = 0.8f;
    health = 2.0f;
}

void HatchlingShip::tick(float delta) {

    EnemyShip::tick(delta);

    if(shouldBeRemoved())
        return;

    sf::Vector2f nearestFriendly = space.getNearestFriendly(location);

    sf::Vector2f distanceToNearestFriendly = nearestFriendly - location;

    if (distanceToNearestFriendly.lengthSq() < min_friendly_target_range) {
        if (tractorBeam) {
            delete tractorBeam;
            tractorBeam = nullptr;
        }
        attackFriendly(distanceToNearestFriendly);
        return;
    }
    
    if(targetAsteroid && !targetAsteroid->isPlanted(targetPlant))
        targetAsteroid = nullptr;
    
    if(!targetAsteroid) {
        // find a target crop
        Crop* closestCrop = nullptr;

        std::vector<Entity*> closeEntities = space.getAllEntitiesInRect(location, {plant_search_range, plant_search_range});

        for (Entity* entity : closeEntities) {
            if (Crop* potentialCrop = dynamic_cast<Crop*>(entity)) {
                
                if(potentialCrop->shouldBeRemoved())
                    continue;
                
                if (closestCrop == nullptr) {
                    closestCrop = potentialCrop;
                    continue;
                }
                
                float dx = closestCrop->getLocation().x - location.x;
                float dy = closestCrop->getLocation().y - location.y;

                float dx2 = potentialCrop->getLocation().x - location.x;
                float dy2 = potentialCrop->getLocation().y - location.y;
                
                float distanceToClosestCrop = dx * dx + dy * dy;
                float distanceToPotentialCrop = dx2 * dx2 + dy2 * dy2;

                if (distanceToPotentialCrop < distanceToClosestCrop) {
                    closestCrop = potentialCrop;
                }
            }
        }
        if(closestCrop) {
            targetAsteroid = &closestCrop->getAsteroid();
            targetPlant = closestCrop->getLocationOnAsteroid();   
        }
    }
    
    if(targetAsteroid == nullptr) {
        attackFriendly(distanceToNearestFriendly);
        return;
    }
    
    Crop* crop = targetAsteroid->getCrop(targetPlant);
    
    if(crop == nullptr) {
        targetAsteroid = nullptr;
        attackFriendly(distanceToNearestFriendly);
        return;
    }

    sf::Vector2 distanceToCrop = crop->getLocation() - location;
    if(distanceToCrop.lengthSq() < 1.0f)  { // todo magic num
        
        crop->damage(this->damageAmount * delta / 1000.0f);
        if (tractorBeam == nullptr) {
            tractorBeam = new TractorBeam(space);
        }
        
        if (crop->shouldBeRemoved()) {
            targetAsteroid = nullptr;
            delete tractorBeam;
            tractorBeam = nullptr;
        }

        if(tractorBeam) {
            tractorBeam->setLocation(location + velocityNormalized);
            tractorBeam->setRotationDegrees(this->rotation);
        }

    } else if (tractorBeam) {
        delete tractorBeam;
        tractorBeam = nullptr;
    }

    velocityNormalized = distanceToCrop.normalized();

    // move towards crop
    if(distanceToCrop.lengthSq() >= 0.9f)
        this->location += velocityNormalized * speed;
    this->rotation = velocityNormalized.angle().asDegrees() + 90.0f;
}

void HatchlingShip::draw(sf::RenderTarget &target, const sf::RenderStates &states) const {
    EnemyShip::draw(target, states);

    if(tractorBeam)
        tractorBeam->draw(target, states);
}

float HatchlingShip::getZOrder() const {
    return 2.0f;
}

void HatchlingShip::attackFriendly(sf::Vector2f distanceToFriendly) {
    fire();

    velocityNormalized = distanceToFriendly.normalized();

    if(distanceToFriendly.lengthSq() >= 7.0f)
        this->location += velocityNormalized * speed;
    this->rotation = velocityNormalized.angle().asDegrees() + 90.0f;
}