#include "ConcretePowerUps.h"
#include "ResourceManager.h"

BasePowerUp::BasePowerUp(float x, float y, sf::Color color) {
    mShape.setPosition(sf::Vector2f(x, y));
    mShape.setRadius(20.0f);
    mShape.setOrigin(sf::Vector2f(20.0f, 20.0f));
    mShape.setFillColor(color);
}

void BasePowerUp::update(sf::Time dt, float gameSpeed) {
    mShape.move(sf::Vector2f(0, gameSpeed * dt.asSeconds()));
    mShape.rotate(90.0f * dt.asSeconds());
}

void BasePowerUp::draw(sf::RenderWindow& window) { window.draw(mShape); }

sf::FloatRect BasePowerUp::getBounds() const {
    return mShape.getGlobalBounds();
}

bool BasePowerUp::isRemovable() const { return mShape.getPosition().y > 800; }

void BasePowerUp::setPosition(sf::Vector2f pos) { mShape.setPosition(pos); }

sf::Vector2f BasePowerUp::getPosition() const { return mShape.getPosition(); }

// --- Implementations ---

MagnetPower::MagnetPower(float x, float y)
    : BasePowerUp(x, y, sf::Color::Red) {
    mShape.setTexture(&ResourceManager::get().getTexture("magnet"));
}
void MagnetPower::applyEffect(Player& player) { player.activateMagnet(10.0f); }

JetpackPower::JetpackPower(float x, float y)
    : BasePowerUp(x, y, sf::Color::Cyan) {
    mShape.setTexture(&ResourceManager::get().getTexture("jetpack"));
}
void JetpackPower::applyEffect(Player& player) {
    player.activateShield(5.0f); // Invincible during flight
    player.jump();               // Simulate flight start
}

ShieldPower::ShieldPower(float x, float y)
    : BasePowerUp(x, y, sf::Color::Blue) {
    mShape.setTexture(&ResourceManager::get().getTexture("shield"));
}
void ShieldPower::applyEffect(Player& player) { player.activateShield(10.0f); }

DoubleCoinPower::DoubleCoinPower(float x, float y)
    : BasePowerUp(x, y, sf::Color::Green) {
    mShape.setTexture(&ResourceManager::get().getTexture("doublecoin"));
}
void DoubleCoinPower::applyEffect(Player& player) {
    player.activateDoubleCoin(10.0f);
}
