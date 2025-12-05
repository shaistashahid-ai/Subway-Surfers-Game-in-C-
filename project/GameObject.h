#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
public:
	virtual ~GameObject() = default;
	virtual void update(sf::Time dt, float gameSpeed) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual sf::FloatRect getBounds() const = 0;
	virtual bool isRemovable() const = 0;
	virtual void setPosition(sf::Vector2f pos) = 0;
	virtual sf::Vector2f getPosition() const = 0;
};
#pragma once
