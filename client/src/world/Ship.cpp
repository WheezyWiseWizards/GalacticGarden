//
// Created by cedric on 1/6/23.
//

#include <algorithm>
#include "world/Space.h"
#include "world/Ship.h"
#include "GameAssets.h"
#include "world/weapon/SmallLaser.h"
#include "world/crop/Crop.h"
#include "world/Asteroid.h"
#include "world/Seed.h"
#include "util/MathUtil.h"
#include "util/SpriteUtil.h"
#include "world/weapon/Bomb.h"

Ship::Ship(Space& space, const sf::Vector2f& location) 
	: Entity(space, location) {
	
	this->sprite.setTexture(*space.getAssets().get(GameAssets::TEXTURE_SHIP), true);
    SpriteUtil::setSpriteOrigin(sprite, { 0.5f, 0.5f });
    float origin_y_pos = (sprite.getTexture()->getSize().y / 2.0f);
    normalAnimeSprite.setOrigin({sprite.getTexture()->getSize().x / 2.0f, origin_y_pos});
    boostAnimeSprite.setOrigin({sprite.getTexture()->getSize().x / 2.0f, origin_y_pos});

    normalAnime.insertFrame(space.getAssets().get(GameAssets::TEXTURE_SHIP_MOVING_1));
    normalAnime.insertFrame(space.getAssets().get(GameAssets::TEXTURE_SHIP_MOVING_2));
    normalAnime.setAnimationSprite(&normalAnimeSprite);

    boostAnime.insertFrame(space.getAssets().get(GameAssets::TEXTURE_SHIP_BOOSTING_1));
    boostAnime.insertFrame(space.getAssets().get(GameAssets::TEXTURE_SHIP_BOOSTING_2));
    boostAnime.setAnimationSprite(&boostAnimeSprite);

    normalAnime.startAnimation();
    boostAnime.startAnimation();

    damageShader = space.getAssets().get(GameAssets::DAMAGE_SHADER);
}

void Ship::tick(float delta) {
    float bad_delta = delta / 1000.f;

    boostAnime.runAnimation(bad_delta);
    normalAnime.runAnimation(bad_delta);

    constexpr float energy_per_boost = 2.f;
    if(isBoosting && !isIdle)
    {
        isBoosting = energy > energy_per_boost * bad_delta;
        if(isBoosting)
            energy -= energy_per_boost * bad_delta;
    }

    if(isBoosting)
    {
        if(this->moveVelocity.length() > maxSpeedBoost)
        {
            this->moveVelocity = this->moveVelocity.normalized() * maxSpeedBoost;
        }
    }
    else if(this->moveVelocity.length() > maxSpeed){
        this->moveVelocity = this->moveVelocity.normalized() * maxSpeed;
   }

    sf::Vector2f newPos = { moveVelocity.x * bad_delta + this->location.x, moveVelocity.y * bad_delta + this->location.y };

    if(newPos.lengthSq() > MathUtil::pow2(Space::MAP_RADIUS)) {
        newPos = newPos.normalized() * Space::MAP_RADIUS;
    }
    
    this->location = newPos;
    time_since_last_fire += bad_delta;
    time_since_last_plant += bad_delta;
    
    for(Entity* entity : space.getAllEntitiesInRect(location, { 2.0f, 2.0f })) {
        Crop* crop = dynamic_cast<Crop*>(entity);
        if(crop && crop->isReady() && (crop->getLocation() - location).lengthSq() < 1.0f) {
            crop->harvest();
            energy += crop->getEnergyGain();
        }
    }

    if (redness > 0.0f) {
        redness -= delta / 1000;
    }

    for(Entity* entity : space.getAllEntitiesInRect(location, { 1.0f, 1.0f })) {
        if(entity->shouldBeRemoved())
            continue;

        if(Lazer* lazer = dynamic_cast<Lazer*>(entity)) {
            if (lazer->getFraction() != fraction) {
                redness = 1.0f;
                energy -= lazer->getDamage()*10;
                lazer->consume();
            }
        }
    }

    if (energy <= .0f)
        space.gameover = true;
}

float Ship::getRotation() const {
    return rotation;
}

void Ship::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
	sprite.setPosition(location);
	sprite.setScale({ 1.0f / sprite.getTexture()->getSize().x, 1.0f / sprite.getTexture()->getSize().y });
    sprite.setRotation(sf::degrees(rotation));

    normalAnimeSprite.setPosition(location);
    normalAnimeSprite.setScale({ 1.0f / sprite.getTexture()->getSize().x, 1.0f / sprite.getTexture()->getSize().y });
    normalAnimeSprite.setRotation(sf::degrees(rotation));

    boostAnimeSprite.setPosition(location);
    boostAnimeSprite.setScale({ 1.0f / sprite.getTexture()->getSize().x, 1.0f / sprite.getTexture()->getSize().y });
    boostAnimeSprite.setRotation(sf::degrees(rotation));

    if(isIdle)
    {
        damageShader->setUniform("hit_multiplier", redness);
        if (redness > 0.0f)
            target.draw(sprite, damageShader);
        else
            target.draw(sprite);
    }
    else if(isBoosting)
    {
        damageShader->setUniform("hit_multiplier", redness);
        if (redness > 0.0f)
            target.draw(boostAnimeSprite, damageShader);
        else
            target.draw(boostAnimeSprite);
    }
    else
    {
        damageShader->setUniform("hit_multiplier", redness);
        if (redness > 0.0f)
            target.draw(normalAnimeSprite, damageShader);
        else
            target.draw(normalAnimeSprite);
    }
}

float Ship::getZOrder() const {
    return 8.f;
}

void Ship::moveInDirOfVec(const sf::Vector2f& moveVec, float good_delta) {
    sf::Vector2f moveVecNorm { 0.f, 0.f };
    
    if(moveVec.length() > 0.001f) // is not zero
        moveVecNorm = moveVec.normalized();

    float current_acc = isBoosting ? boostAcc : acc;
    this->moveVelocity += moveVecNorm * current_acc * good_delta;
}


void Ship::fire() {

    if(time_since_last_fire >= fire_delay)
    {
        switch(weaponType) {

            case SIMPLE:
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                break;
            case DOUBLE:
                if(!energy_for_shot(1)) return;
                space.addEntity(new SmallLaser(space, location + (sf::Vector2f {0.25f, 0.0f}).rotatedBy(sf::degrees(rotation)), sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                space.addEntity(new SmallLaser(space, location + (sf::Vector2f {-0.25f, 0.0f}).rotatedBy(sf::degrees(rotation)), sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                break;
            case TRIANGLE:
                if(!energy_for_shot(1)) return;
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation - 15.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 15.0f))));
                break;
            case FOUR_WAY:
                if(!energy_for_shot(1)) return;
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 90.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 180.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 270.0f))));
                break;
            case CRAZY:
                if(!energy_for_shot(5)) return;
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 90.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 180.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 270.0f))));
                space.addEntity(new SmallLaser(space, location + (sf::Vector2f {0.25f, 0.0f}).rotatedBy(sf::degrees(rotation)), sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                space.addEntity(new SmallLaser(space, location + (sf::Vector2f {-0.25f, 0.0f}).rotatedBy(sf::degrees(rotation)), sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation - 15.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 15.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation - 30.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 30.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation - 45.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 45.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation - 60.0f))));
                space.addEntity(new SmallLaser(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation + 60.0f))));
                break;
            case BOMB:
                if(!energy_for_shot(5)) return;
                space.addEntity(new Bomb(space, location, sf::Vector2f(0.f, -1.0f).rotatedBy(sf::degrees(rotation))));
                break;
                
        }
        
        time_since_last_fire = 0.f;
    }
}

void Ship::setRotation(float rotationRad) {
    this->rotation = rotationRad;
}

void Ship::plantOnAsteroid(Space& space) {
    if(time_since_last_plant >= plant_delay) {
        std::vector<plantzone_t> seedrics;
        std::vector<Entity *> entities = space.getAllEntitiesInRect(this->location, {6, 6});

        for (Entity *entity: entities) {
            Asteroid *asteroid = dynamic_cast<Asteroid *>(entity);

            if (asteroid == nullptr)
                continue;

            std::vector<plantzone_t> cedrics = getClosestAvailablePlantingZones(*asteroid);

            for(const auto& cedric : cedrics)
            {
                seedrics.push_back(cedric);
            }
        }
        std::sort(seedrics.begin(), seedrics.end(), [&](const plantzone_t& a, const plantzone_t & b){
            float distanceA = ((a.second->getLocation() + a.first.rotatedBy(sf::degrees(a.second->getRotation()))) - this->location).lengthSq();
            float distanceB = ((a.second->getLocation() + b.first.rotatedBy(sf::degrees(a.second->getRotation()))) - this->location).lengthSq();

            return distanceA < distanceB;
        });

        if(!seedrics.empty()) {
            for(auto& zone : seedrics) {
                if(!seed_thrown.contains(zone) || !seed_thrown[zone]) {
                   seed_thrown[zone] = true;

                    Seed* newSeed = new Seed(space, location, zone.second, zone.first);
                    newSeed->setRotationDegrees(this->rotation);
                    this->space.addEntity((newSeed));

                    time_since_last_plant = .0f;
                   break;
                }
            }
        }
    }
}

std::vector<plantzone_t> Ship::getClosestAvailablePlantingZones(Asteroid& asteroid) {
    std::vector<plantzone_t> ret;

    for (const auto &plantingLocation : asteroid.getPlantingLocations()) {
        if (!asteroid.isPlanted(plantingLocation))
            ret.emplace_back(plantingLocation, &asteroid);
    }

    std::sort(ret.begin(), ret.end(), [&](const plantzone_t& a, const plantzone_t & b){
        float distanceA = ((asteroid.getLocation() + a.first.rotatedBy(sf::degrees(asteroid.getRotation()))) - this->location).lengthSq();
        float distanceB = ((asteroid.getLocation() + b.first.rotatedBy(sf::degrees(asteroid.getRotation()))) - this->location).lengthSq();

        return distanceA < distanceB;
    });

    return ret;
}

void Ship::setWeaponType(WeaponType weaponType) {
    this->weaponType = weaponType;
}

bool Ship::energy_for_shot(int shot_count) {
    int energy_per_shot = 1;
    int energy_used = shot_count * energy_per_shot;
    bool success = energy_used < energy;
    if(success)
        energy -= energy_used;
    return success;
}

int Ship::getEnergy() const {
    return energy;
}

std::map<plantzone_t, bool, PlantZoneCompare> &Ship::getSeedThrown() {
    return seed_thrown;
}

void Ship::setIsBoosting(bool isBoosting) {
    this->isBoosting = isBoosting;
}

void Ship::setIsIdle(bool isIdle) {
    this->isIdle = isIdle;
}

const sf::Vector2f& Ship::getMoveVelocity() const {
    return moveVelocity;
}
