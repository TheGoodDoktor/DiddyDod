// Entity Manager
#include "EntityManager.h"

#include <vector>
#include <deque>
#include <map>
#include <assert.h>



C_Entity C_EntityManager::CreateEntity()
{
	U32 index = 0;

	if (m_FreeIndices.size() > kMinimumFreeIndices)	// use a free index?
	{
		index = m_FreeIndices.front();
		m_FreeIndices.pop_front();
	}
	else
	{
		m_Generation.push_back(0);
		index = (U32)m_Generation.size() - 1;
		assert(index < (1 << kEntityIndexBits));
	}

	return C_Entity::Make(index, m_Generation[index]);
}

void C_EntityManager::Destroy(C_Entity entity)
{
	const U32 index = entity.Index();
	++m_Generation[index];
	m_FreeIndices.push_back(index);
}



