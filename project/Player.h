#pragma once
#include "LaneSystem.h"
#pragma once
#include "LaneSystem.h"
#include <SFML/Graphics.hpp>

enum class PlayerState { RUNNING, JUMPING, SLIDING };

class Player {
public:
    Player();

    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);

    void moveLeft();
    void moveRight();
    void jump();
    void slide();

    sf::FloatRect getBounds() const;
    PlayerState getState() const { return mState; }
    int getLane() const { return mLane; }
	bool isRunning() const { return mState == PlayerState::RUNNING; }
	bool isJumping() const { return mState == PlayerState::JUMPING; }
	bool isSliding() const { return mState == PlayerState::SLIDING; }

    void setInvincible(bool invincible) { mIsInvincible = invincible; }
    bool isInvincible() const { return mIsInvincible; }

    void activateMagnet(float duration) { mMagnetTimer = duration; }
    bool isMagnetActive() const { return mMagnetTimer > 0; }

    void activateDoubleCoin(float duration) { mDoubleCoinTimer = duration; }
    bool isDoubleCoinActive() const { return mDoubleCoinTimer > 0; }

    void activateShield(float duration) {
        mInvincibleTimer = duration;
        mIsInvincible = true;
    }

private:
    sf::RectangleShape mShape;
    int mLane; // 0, 1, 2
    float mCurrentX;

    PlayerState mState;
    float mVerticalVelocity;
    float mGroundY;

    // Jump constants
    const float GRAVITY = 1500.0f;
    const float JUMP_FORCE = -800.0f;

    // Slide constants
    float mSlideTimer;
    const float SLIDE_DURATION = 1.0f;

    // Movement smoothing
    const float LANE_SWITCH_SPEED = 15.0f;

    bool mIsInvincible;
    float mInvincibleTimer;
    float mMagnetTimer;
    float mDoubleCoinTimer;
};
#pragma once
