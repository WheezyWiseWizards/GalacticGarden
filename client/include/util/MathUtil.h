//
// Created by Alexander Winter on 2023-01-07.
//

#ifndef LD52_CLIENT_MATHUTIL_H
#define LD52_CLIENT_MATHUTIL_H

#include "SFML/System/Vector2.hpp"

namespace MathUtil
{
    inline sf::Vector2f toFloat(sf::Vector2i intVec) {
        return sf::Vector2f { static_cast<float>(intVec.x), static_cast<float>(intVec.y) };
    }
    inline sf::Vector2f toFloat(sf::Vector2u intVec) {
        return sf::Vector2f { static_cast<float>(intVec.x), static_cast<float>(intVec.y) };
    }
}

#endif //LD52_CLIENT_MATHUTIL_H
