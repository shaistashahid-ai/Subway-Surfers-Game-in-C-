#pragma once
#include "GameObject.h"
#include "Player.h"

enum class ObstacleType { TRAIN, BARRIER, CONE, FENCE };

class Obstacle : public GameObject {
public:
	virtual bool onCollision(Player& player) = 0; // return true if collision should end the game
	virtual ObstacleType getType() const = 0;
};
#pragma once
