#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "Types.h"
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>

const U32 kEntityIndexBits = 24;
const U32 kEntityIndexMask = (1 << kEntityIndexBits) - 1;

const U32 kEntityGenerationBits = 8;
const U32 kEntityGenerationMask = (1 << kEntityGenerationBits) - 1;

// Entity Handle
// Essentially a weak reference
struct C_Entity
{
	U32 id;

	U32 Index() const { return id & kEntityIndexMask; }
	U32 Generation() const { return (id >> kEntityIndexBits) & kEntityGenerationMask; }


	//bool operator< (C_Entity other)const { return id < other.id; }	//so we can use a map
	bool operator== (C_Entity other)const { return id == other.id; }	//so we can use a map


	static C_Entity Make(int index, int generation)
	{
		return { (index & kEntityIndexMask) | ((generation & kEntityGenerationMask) << kEntityIndexBits) };
	}
};

// hashing function - just use id as it is unique
namespace std 
{
	template <>
	struct hash<C_Entity>
	{
		std::size_t operator()(const C_Entity& k) const
		{
			return (std::size_t)k.id;
		}
	};
}

// use hashmap
typedef std::unordered_map <C_Entity, U32> C_EntityIndexMap;

// Entity Manager
// Managers lifetime of entities
class C_EntityManager
{
public:

	static const int kMinimumFreeIndices = 1000;

	C_Entity	CreateEntity();
	void		Destroy(C_Entity entity);

	bool IsAlive(C_Entity entity) const	{ return m_Generation[entity.Index()] == entity.Generation();	}

private:
	std::vector<U8>	m_Generation;	// Generation counter for entity indices
	std::deque<U32>	m_FreeIndices;	// Free indices that can be used again
};


#endif