#include "ConcreteObstacles.h"
#include "ResourceManager.h"
using namespace std;
BaseObstacle::BaseObstacle(float x, float y, float width, float height,
    sf::Color color) {
    mShape.setPosition(sf::Vector2f(x, y));
    mShape.setSize(sf::Vector2f(width, height));
    mShape.setFillColor(color);
    mShape.setOrigin(
        sf::Vector2f(width / 2.0f, height)); // Origin at bottom center
}

void BaseObstacle::update(sf::Time dt, float gameSpeed) {
    mShape.move(sf::Vector2f(0, gameSpeed * dt.asSeconds()));
}

void BaseObstacle::draw(sf::RenderWindow& window) { window.draw(mShape); }

sf::FloatRect BaseObstacle::getBounds() const {
    return mShape.getGlobalBounds();
}

bool BaseObstacle::isRemovable() const {
    return mShape.getPosition().y > 800; // Screen height + buffer
}

void BaseObstacle::setPosition(sf::Vector2f pos) { mShape.setPosition(pos); }

sf::Vector2f BaseObstacle::getPosition() const { return mShape.getPosition(); }

// --- Concrete Implementations ---

Train::Train(float x, float y)
    : BaseObstacle(x, y, 100.0f, 200.0f, sf::Color::White) {
    auto& rm = ResourceManager::get();
    if (rm.hasTexture("trainFull")) {
        mShape.setTexture(&rm.getTexture("trainFull"));
    }
    else if (rm.hasTexture("train")) {
        mShape.setTexture(&rm.getTexture("train"));
    }
}
bool Train::onCollision(Player& player) {
    return !player.isInvincible();
}

Barrier::Barrier(float x, float y)
    : BaseObstacle(x, y, 120.0f, 80.0f, sf::Color::White) {
    auto& rm = ResourceManager::get();
    if (rm.hasTexture("barrier")) {
        mShape.setTexture(&rm.getTexture("barrier"));
    }
} // Orange
bool Barrier::onCollision(Player& player) {
    if (player.isInvincible()) {
        return false;
    }
    sf::FloatRect playerBounds = player.getBounds();
    sf::FloatRect obstacleBounds = mShape.getGlobalBounds();
    float playerFeet = playerBounds.top + playerBounds.height;
    float barrierTop = obstacleBounds.top;
    float barrierBottom = obstacleBounds.top + obstacleBounds.height;

    if (player.isSliding() && playerBounds.height <= 60.f) {
        return false;
    }
    if (player.isJumping()) {
        const float clearance = 60.f;
        if (playerFeet <= barrierTop + clearance) {
            return false;
        }
        float playerMid = playerBounds.top + playerBounds.height * 0.5f;
        if (playerMid <= barrierTop + 20.f) {
            return false;
        }
        if (playerBounds.top + playerBounds.height <= barrierTop + 15.f) {
            return false;
        }
       // cout<<"it barrier enters\n";
        return false;
    }
       // cout<<"it barrier no enters\n";
    return true;
}

Cone::Cone(float x, float y)
    : BaseObstacle(x, y, 40.0f, 40.0f, sf::Color::White) {
    auto& rm = ResourceManager::get();
    if (rm.hasTexture("cone")) {
        mShape.setTexture(&rm.getTexture("cone"));
    }
}
bool Cone::onCollision(Player& player) {
    if (player.isInvincible()) {
        return false;
    }
    if (player.isJumping()) {
        sf::FloatRect playerBounds = player.getBounds();
        sf::FloatRect obstacleBounds = mShape.getGlobalBounds();
        float playerFeet = playerBounds.top + playerBounds.height;
        float coneTop = obstacleBounds.top;
        if (playerFeet <= coneTop + 10.f) {
            return false;
        }
        if (playerBounds.top + 20.f <= coneTop) {
            return false;
        }
       // cout<<"it cone enters\n";
        return false;
    }
       // cout<<"it cone no enters\n";
    return true;
}

Fence::Fence(float x, float y)
    : BaseObstacle(x, y, 150.0f, 100.0f, sf::Color::White) {
    auto& rm = ResourceManager::get();
    if (rm.hasTexture("fence")) {
        mShape.setTexture(&rm.getTexture("fence"));
    }
} // Brown
bool Fence::onCollision(Player& player) {
    if (player.isInvincible()) {
        return false;
    }
    if (player.isSliding()) {
        sf::FloatRect playerBounds = player.getBounds();
        if (playerBounds.height <= 60.f) {
            return false;
        }
    }
    if (player.isJumping()) {
        sf::FloatRect playerBounds = player.getBounds();
        sf::FloatRect obstacleBounds = mShape.getGlobalBounds();
        float playerFeet = playerBounds.top + playerBounds.height;
        float fenceTop = obstacleBounds.top;
        if (playerFeet <= fenceTop + 15.f) {
            return false;
        }
        if (playerBounds.top + 20.f <= fenceTop) {
            return false;
        }
        //cout<<"it fence enters\n";
        return true;
    }
        //cout<<"it barrier no enters\n";
    return true;
}
