#pragma once
#include "Coin.h"
#include "ConcreteObstacles.h"
#include "ConcretePowerUps.h"
#include "GameList.h"
#include "Obstacle.h"
#include "PowerUp.h"
#include <SFML/Graphics.hpp>
#include <random>


class TrackManager {
public:
	TrackManager();
	void update(sf::Time dt);
	void draw(sf::RenderWindow& window);

	GameList<Obstacle>& getObstacles() { return mObstacles; }
	GameList<PowerUp>& getPowerUps() { return mPowerUps; }
	GameList<Coin>& getCoins() { return mCoins; }

	float getGameSpeed() const { return mGameSpeed; }
	void setGameSpeed(float speed) { mGameSpeed = speed; }
	void increaseSpeed(float amount) { mGameSpeed += amount; }

private:
	void spawnObstacle();
	void spawnCoin();
	void spawnPowerUp();

	GameList<Obstacle> mObstacles;
	GameList<PowerUp> mPowerUps;
	GameList<Coin> mCoins;

	float mGameSpeed;
	float mSpawnTimer;
	float mCoinTimer;
	float mPowerUpTimer;
	float mDifficultyTimer;
	std::mt19937 mRng;

	float obstacleInterval() const;
	float coinInterval() const;
	float powerUpInterval() const;
	void spawnCoinRow(int lane, int count, float spacing);
	int randomLane(int excludeLane = -1);
	ObstacleType randomObstacleType();
};
#pragma once
