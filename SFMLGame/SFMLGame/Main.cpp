#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

#include "ECSLookup.h"


// Basic Types
typedef float F32;

struct Vec2
{
	F32 x, y;
};

// Components

enum E_ComponentType
{
	E_ComponentType_Transform = 0,
	E_ComponentType_Sprite,

	E_ComponentType_Count
};

struct C_TransformComponent : public C_Component
{
	Vec2		m_Position;
	F32			m_Rotation;
};

struct C_SpriteComponent : public C_Component
{
	sf::Sprite m_Sprite;
};

void SetSpritePosition(C_SpriteComponent &spriteComp, const C_TransformComponent &transformComp)
{
	spriteComp.m_Sprite.setPosition(transformComp.m_Position.x, transformComp.m_Position.y);
	spriteComp.m_Sprite.setRotation(transformComp.m_Rotation);
}

// For each entity type have an array of each component the entity uses (structure of arrays)
// example:
struct MyEntityType
{
	std::vector<C_TransformComponent> m_Transforms;
	std::vector<C_SpriteComponent> m_Sprites;
	int m_NoEntities;	// total number of entities of this type

	// Set the positions of all sprites
	void SetSpritePositions()
	{
		for (int i = 0; i < m_NoEntities; i++)
			SetSpritePosition(m_Sprites[i], m_Transforms[i]);
	}
};

// We need a system for managing arrays with holes in them
struct C_ArrayManager
{
	C_ArrayManager(int noElements)
	{
		const int bitfieldElements = (noElements / 32) + 1;
		m_ValidElements = new U32[bitfieldElements];
		for (int i = 0; i < bitfieldElements; i++)
			m_ValidElements[i] = 0;

		m_NoElements = bitfieldElements;
	}

	int	GetNewIndex()
	{
		// pop free index? or iterate & get the bit
		return -1;
	}

	void FreeIndex(int index)
	{
		m_ValidElements[index / 32] &= ~(1 << (index % 32));	// free bit
		// push free index?
	}

	bool IsIndexValid(int index)
	{
		return (m_ValidElements[index / 32] & ~(1 << (index % 32))) != 0;
	}

	int m_NoElements;
	U32 *	m_ValidElements;	// bitfield of valid elements
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFMLGame");
	sf::CircleShape shape(100.0f);

	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
		sf::Event event;
		 
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}
}