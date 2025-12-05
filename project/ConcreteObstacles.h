#pragma once
#include "Obstacle.h"

class BaseObstacle : public Obstacle {
public:
	BaseObstacle(float x, float y, float width, float height, sf::Color color);
	void update(sf::Time dt, float gameSpeed) override;
	void draw(sf::RenderWindow& window) override;
	sf::FloatRect getBounds() const override;
	bool isRemovable() const override;
	void setPosition(sf::Vector2f pos) override;
	sf::Vector2f getPosition() const override;

protected:
	sf::RectangleShape mShape;
};

class Train : public BaseObstacle {
public:
	Train(float x, float y);
	bool onCollision(Player& player) override;
	ObstacleType getType() const override { return ObstacleType::TRAIN; }
};

class Barrier : public BaseObstacle {
public:
	Barrier(float x, float y);
	bool onCollision(Player& player) override;
	ObstacleType getType() const override { return ObstacleType::BARRIER; }
};

class Cone : public BaseObstacle {
public:
	Cone(float x, float y);
	bool onCollision(Player& player) override;
	ObstacleType getType() const override { return ObstacleType::CONE; }
};

class Fence : public BaseObstacle {
public:
	Fence(float x, float y);
	bool onCollision(Player& player) override;
	ObstacleType getType() const override { return ObstacleType::FENCE; }
};
#pragma once
