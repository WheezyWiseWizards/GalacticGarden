//
// Created by Winter on 12/02/2022.
//

#include "GalacticGarden.h"

#include <utility>

GalacticGarden::GalacticGarden(std::shared_ptr<sf::RenderWindow> window,
						 std::shared_ptr<wiz::Logger> logger,
						 std::shared_ptr<wiz::AssetLoader> assetLoader)
	: BasicGame(std::move(window), std::move(logger), std::move(assetLoader)) {}

const std::string &GalacticGarden::getJoyStickName() const {
    return joyStickName;
}

void GalacticGarden::setJoyStickName(const sf::String joyStickName) {
    GalacticGarden::joyStickName = joyStickName;
}
