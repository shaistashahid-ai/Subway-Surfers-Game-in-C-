#pragma once
#include "PowerUp.h"

class BasePowerUp : public PowerUp {
public:
	BasePowerUp(float x, float y, sf::Color color);
	void update(sf::Time dt, float gameSpeed) override;
	void draw(sf::RenderWindow& window) override;
	sf::FloatRect getBounds() const override;
	bool isRemovable() const override;
	void setPosition(sf::Vector2f pos) override;
	sf::Vector2f getPosition() const override;

protected:
	sf::CircleShape mShape;
};

class MagnetPower : public BasePowerUp {
public:
	MagnetPower(float x, float y);
	void applyEffect(Player& player) override;
	PowerUpType getType() const override { return PowerUpType::MAGNET; }
};

class JetpackPower : public BasePowerUp {
public:
	JetpackPower(float x, float y);
	void applyEffect(Player& player) override;
	PowerUpType getType() const override { return PowerUpType::JETPACK; }
};

class ShieldPower : public BasePowerUp {
public:
	ShieldPower(float x, float y);
	void applyEffect(Player& player) override;
	PowerUpType getType() const override { return PowerUpType::SHIELD; }
};

class DoubleCoinPower : public BasePowerUp {
public:
	DoubleCoinPower(float x, float y);
	void applyEffect(Player& player) override;
	PowerUpType getType() const override { return PowerUpType::DOUBLE_COIN; }
};
#pragma once
