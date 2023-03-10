//
// Created by Alexander Winter on 2023-01-08.
//

#include "world/weapon/Bomb.h"
#include "GameAssets.h"
#include "world/Space.h"
#include "util/SpriteUtil.h"
#include "world/enemy/EnemyShip.h"
#include "util/MathUtil.h"
#include "world/FloatingText.h"

Bomb::Bomb(Space& space, sf::Vector2f location, sf::Vector2f direction) 
    : Entity(space, location), velocity(direction * 5.0f) {
    bombSprite.setTexture(*space.getAssets().get(GameAssets::TEXTURE_BOMB));
    explosionSprite.setTexture(*space.getAssets().get(GameAssets::TEXTURE_EXPLOSION_PARTICLE));

    SpriteUtil::setSpriteOrigin(bombSprite, { 0.5f, 0.5f });
    SpriteUtil::setSpriteOrigin(explosionSprite, { 0.5f, 0.5f });
    SpriteUtil::setSpriteSize(bombSprite, { 0.5f, 1.0f });

    rotation = direction.angle().asDegrees() + 90.0f;
}

void Bomb::tick(float delta) {
    if(explosionTime >= 0.0f) {
        explosionTime += delta / 1000.0f;
        
        for(particle_t& particle : particles) {
            std::get<0>(particle) = std::get<0>(particle) + (std::get<0>(particle) - location).normalized() * delta / 1000.0f * std::get<4>(particle);
            std::get<2>(particle) = std::get<2>(particle) + std::get<3>(particle) * delta / 1000.0f; 
        }
        
        return;
    }
    
    location += velocity * delta / 1000.0f;
    lifetime -= delta / 1000.0f;

    for(Entity* entity : space.getAllEntitiesInRect(location, { 1.0f, 1.0f })) {
        if(EnemyShip* ship = dynamic_cast<EnemyShip*>(entity)) {
            if((ship->getLocation() - location).lengthSq() < MathUtil::pow2(1.0f)) {
                lifetime = 0.0f;
                break;
            }
        }
    }
    
    if(lifetime <= 0.0f) {
        lifetime = 0.0f;
        explosionTime = 0.0f;
        space.getShip().nukeSound.play();
        
        int count = 20 + rand() % 20;
        
        for(int i = 0; i < count; i++) {
            float angle = i * 360.0f / count + (rand() / (RAND_MAX + 1.0f) * 2.0f - 1.0f) * 10.0f;
            float size = rand() / (RAND_MAX + 1.0f) * 2.0f + 0.5f;
            float rot = rand() / (RAND_MAX + 1.0f) * 360.0f;
            float angVel = rand() / (RAND_MAX + 1.0f) * 100.0f;
            float speed = rand() / (RAND_MAX + 1.0f) * 4.0f;
            
            particles.emplace_back(location + sf::Vector2f { 0.0f, 1.0f }.rotatedBy(sf::degrees(angle)), size, rot, angVel, speed);
        }
        
        for(Entity* entity : space.getAllEntitiesInRect(location, { 10.0f, 10.0f })) {
            if(EnemyShip* ship = dynamic_cast<EnemyShip*>(entity)) {
                if((ship->getLocation() - location).lengthSq() < MathUtil::pow2(4.0f)) {
                    ship->damage(10.0f);

                    space.addEntity(new FloatingText(space, ship->getLocation(), "-10", sf::Color::Yellow, 0.5f));
                }
            }
        }
    }
}

void Bomb::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
    if(explosionTime < 0.0f) {
        bombSprite.setPosition(location);
        bombSprite.setRotation(sf::degrees(rotation));
        target.draw(bombSprite);
    } else {
        float alpha = opacity(explosionTime);
        explosionSprite.setColor(sf::Color(255, 255, 255, round(alpha * 128)));
        
        for(particle_t particle : particles) {
            explosionSprite.setPosition(std::get<0>(particle));
            SpriteUtil::setSpriteSize(explosionSprite, { std::get<1>(particle), std::get<1>(particle) });
            explosionSprite.setRotation(sf::degrees(std::get<2>(particle)));
            target.draw(explosionSprite);   
        }
    }
}

float Bomb::opacity(float value) {
    value *= 5.0f;
    
    if(value <= 1.0f)
        return value * value;
    
    if(value <= 5.0f)
        return 1.0f;
    
    return fmax(0.0f, 1.0f - (value - 5.0f));
}

bool Bomb::shouldBeRemoved() const {
    return explosionTime >= 2.0f;
}

sf::Vector2f Bomb::getVisualSize() const {
    return { 5.0f, 5.0f };
}

float Bomb::getZOrder() const {
    return 5.0f;
}
