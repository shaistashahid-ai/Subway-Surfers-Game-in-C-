#include "Player.h"
#include "ResourceManager.h"
#include <cmath>

Player::Player()
    : mLane(1), mState(PlayerState::RUNNING), mVerticalVelocity(0.0f),
    mGroundY(500.0f), mSlideTimer(0.0f), mIsInvincible(false),
    mInvincibleTimer(0.0f), mMagnetTimer(0.0f), mDoubleCoinTimer(0.0f) {

    mCurrentX = LaneSystem::getLaneCenter(mLane);

    mShape.setSize(sf::Vector2f(50.0f, 100.0f));
    mShape.setOrigin(sf::Vector2f(25.0f, 100.0f)); // Origin at bottom center
    mShape.setPosition(sf::Vector2f(mCurrentX, mGroundY));
    mShape.setTexture(&ResourceManager::get().getTexture("playerSpritesheet"));
}

void Player::update(sf::Time dt) {
    float dtSeconds = dt.asSeconds();

    // Update PowerUps
    if (mInvincibleTimer > 0) {
        mInvincibleTimer -= dtSeconds;
        if (mInvincibleTimer <= 0)
            mIsInvincible = false;
    }
    if (mMagnetTimer > 0)
        mMagnetTimer -= dtSeconds;
    if (mDoubleCoinTimer > 0)
        mDoubleCoinTimer -= dtSeconds;

    // Horizontal Movement (Lerp)
    float targetX = LaneSystem::getLaneCenter(mLane);
    float diff = targetX - mCurrentX;
    mCurrentX += diff * LANE_SWITCH_SPEED * dtSeconds;

    // Vertical Movement (Jump)
    if (mState == PlayerState::JUMPING) {
        mVerticalVelocity += GRAVITY * dtSeconds;
        float newY = mShape.getPosition().y + mVerticalVelocity * dtSeconds;

        if (newY >= mGroundY) {
            newY = mGroundY;
            mState = PlayerState::RUNNING;
            mVerticalVelocity = 0.0f;
        }
        mShape.setPosition(sf::Vector2f(mCurrentX, newY));
    }
    else {
        mShape.setPosition(sf::Vector2f(mCurrentX, mGroundY));
    }

    // Slide Logic
    if (mState == PlayerState::SLIDING) {
        mSlideTimer -= dtSeconds;
        if (mSlideTimer <= 0) {
            mState = PlayerState::RUNNING;
            mShape.setSize(sf::Vector2f(50.0f, 100.0f)); // Restore size
            mShape.setOrigin(sf::Vector2f(25.0f, 100.0f));
        }
    }
}

void Player::draw(sf::RenderWindow& window) { window.draw(mShape); }

void Player::moveLeft() {
    if (mLane > 0)
        mLane--;
}

void Player::moveRight() {
    if (mLane < LaneSystem::LANE_COUNT - 1)
        mLane++;
}

void Player::jump() {
    if (mState == PlayerState::RUNNING || mState == PlayerState::SLIDING) {
        mState = PlayerState::JUMPING;
        mVerticalVelocity = JUMP_FORCE;
        // Reset size if jumping from slide
        mShape.setSize(sf::Vector2f(50.0f, 100.0f));
        mShape.setOrigin(sf::Vector2f(25.0f, 100.0f));
    }
}

void Player::slide() {
    if (mState == PlayerState::RUNNING) {
        mState = PlayerState::SLIDING;
        mSlideTimer = SLIDE_DURATION;
        mShape.setSize(sf::Vector2f(50.0f, 50.0f)); // Shrink
        mShape.setOrigin(sf::Vector2f(25.0f, 50.0f));
    }
    else if (mState == PlayerState::JUMPING) {
        // Fast fall?
        mVerticalVelocity += 1000.0f;
    }
}

sf::FloatRect Player::getBounds() const { return mShape.getGlobalBounds(); }
