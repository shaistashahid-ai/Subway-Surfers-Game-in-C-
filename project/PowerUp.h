#pragma once
#include "GameObject.h"
#include "Player.h"

enum class PowerUpType { MAGNET, JETPACK, SHIELD, DOUBLE_COIN };

class PowerUp : public GameObject {
public:
	virtual void applyEffect(Player& player) = 0;
	virtual PowerUpType getType() const = 0;
};
#pragma once
