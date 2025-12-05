#pragma once
#include "GameObject.h"
#include "ResourceManager.h"

class Coin : public GameObject {
public:
    Coin(float x, float y) {
        mShape.setRadius(15.0f);
        mShape.setOrigin(sf::Vector2f(15.0f, 15.0f));
        mShape.setPosition(sf::Vector2f(x, y));
        mShape.setFillColor(sf::Color::White);
        mShape.setTexture(&ResourceManager::get().getTexture("coin"));
    }

    void update(sf::Time dt, float gameSpeed) override {
        mShape.move(sf::Vector2f(0, gameSpeed * dt.asSeconds()));
        mShape.rotate(180.0f * dt.asSeconds());
    }

    void draw(sf::RenderWindow& window) override { window.draw(mShape); }

    sf::FloatRect getBounds() const override { return mShape.getGlobalBounds(); }

    bool isRemovable() const override { return mShape.getPosition().y > 800; }

    void setPosition(sf::Vector2f pos) override { mShape.setPosition(pos); }

    sf::Vector2f getPosition() const override { return mShape.getPosition(); }

private:
    sf::CircleShape mShape;
};
#pragma once
