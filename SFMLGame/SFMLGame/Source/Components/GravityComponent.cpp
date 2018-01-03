#include "GravityComponent.h"
#include "Util.h"
#include <assert.h>

void C_GravityComponentManager::Allocate(U32 noToAllocate)
{
	assert(noToAllocate > m_Data.m_NoElements);

	C_InstanceData newData;
	const U32 bytes = noToAllocate * (U32)C_InstanceData::ComponentSize();

	// Allocate buffers
	newData.m_pBuffer = malloc(bytes);
	newData.m_NoElements = m_Data.m_NoElements;
	newData.m_NoAllocated = noToAllocate;

	// Set up pointers
	newData.m_Entities = (C_Entity *)(newData.m_pBuffer);
	newData.m_Gravity = (float *)(newData.m_Entities + noToAllocate);
	newData.m_Pos = (Vec2 *)(newData.m_Gravity + noToAllocate);
	
	// copy over old data
	if (m_Data.m_NoElements != 0)
	{
		memcpy(newData.m_Entities, m_Data.m_Entities, m_Data.m_NoElements * sizeof(C_Entity));
		memcpy(newData.m_Gravity, m_Data.m_Gravity, m_Data.m_NoElements * sizeof(float));
		memcpy(newData.m_Pos, m_Data.m_Pos, m_Data.m_NoElements * sizeof(Vec2));
	}

	if (m_Data.m_pBuffer != nullptr)
		free(m_Data.m_pBuffer);	// free old buffer

	m_Data = newData;	// swap to new data
}

void C_GravityComponentManager::Destroy(U32 index)
{
	const U32 lastIndex = m_Data.m_NoElements - 1;
	const C_Entity entity = m_Data.m_Entities[index];
	const C_Entity lastEntity = m_Data.m_Entities[lastIndex];

	m_Data.m_Entities[index] = m_Data.m_Entities[lastIndex];
	m_Data.m_Gravity[index] = m_Data.m_Gravity[lastIndex];
	m_Data.m_Pos[index] = m_Data.m_Pos[lastIndex];

	m_Map[lastEntity] = index;
	m_Map.erase(entity);

	--m_Data.m_NoElements;
}

void C_GravityComponentManager::GarbageCollect(const C_EntityManager &entityManager)
{
	U32 aliveInrow = 0;

	while (m_Data.m_NoElements > 0 && aliveInrow < 4)
	{
		const U32 i = Util::RandomInRange(0, m_Data.m_NoElements - 1);	// random index - maybe replace with sliding window?
		if (entityManager.IsAlive(m_Data.m_Entities[i]))
		{
			++aliveInrow;
			continue;
		}
		aliveInrow = 0;
		Destroy(i);
	}
}