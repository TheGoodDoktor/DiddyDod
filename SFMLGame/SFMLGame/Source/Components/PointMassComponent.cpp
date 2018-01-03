#include "PointMassComponent.h"
#include "GravityComponent.h"
#include "Util.h"
#include <assert.h>

void C_PointMassComponentManager::Allocate(U32 noToAllocate)
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
	newData.m_Mass = (float *)(newData.m_Entities + noToAllocate);
	newData.m_Pos = (Vec2 *)(newData.m_Mass + noToAllocate);
	newData.m_Vel = newData.m_Pos + noToAllocate;
	newData.m_Accel = newData.m_Vel + noToAllocate;

	// copy over old data
	if (m_Data.m_NoElements != 0)
	{
		memcpy(newData.m_Entities, m_Data.m_Entities, m_Data.m_NoElements * sizeof(C_Entity));
		memcpy(newData.m_Mass, m_Data.m_Mass, m_Data.m_NoElements * sizeof(float));
		memcpy(newData.m_Pos, m_Data.m_Pos, m_Data.m_NoElements * sizeof(Vec2));
		memcpy(newData.m_Vel, m_Data.m_Vel, m_Data.m_NoElements * sizeof(Vec2));
		memcpy(newData.m_Accel, m_Data.m_Accel, m_Data.m_NoElements * sizeof(Vec2));
	}

	if (m_Data.m_pBuffer != nullptr)
		free(m_Data.m_pBuffer);	// free old buffer

	m_Data = newData;	// swap to new data
}

void C_PointMassComponentManager::Destroy(U32 index)
{
	const U32 lastIndex = m_Data.m_NoElements - 1;
	const C_Entity entity = m_Data.m_Entities[index];
	const C_Entity lastEntity = m_Data.m_Entities[lastIndex];

	m_Data.m_Entities[index] = m_Data.m_Entities[lastIndex];
	m_Data.m_Mass[index] = m_Data.m_Mass[lastIndex];
	m_Data.m_Pos[index] = m_Data.m_Pos[lastIndex];
	m_Data.m_Vel[index] = m_Data.m_Vel[lastIndex];
	m_Data.m_Accel[index] = m_Data.m_Accel[lastIndex];

	m_Map[lastEntity] = index;
	m_Map.erase(entity);

	--m_Data.m_NoElements;
}

void C_PointMassComponentManager::GarbageCollect(const C_EntityManager &entityManager)
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

// processing methods
void C_PointMassComponentManager::Simulate(float dT)
{
	for (int i = 0; i < m_Data.m_NoElements; i++)
	{
		// Apply acceleration
		m_Data.m_Vel[i].x += m_Data.m_Accel[i].x * dT;
		m_Data.m_Vel[i].y += m_Data.m_Accel[i].y * dT;

		// Apply velocity
		m_Data.m_Pos[i].x += m_Data.m_Vel[i].x * dT;
		m_Data.m_Pos[i].y += m_Data.m_Vel[i].y * dT;
	}
}

void C_PointMassComponentManager::ApplyGravityForces(const C_GravityComponentManager &gravityComponents)
{
	if (gravityComponents.GetNoInstances() == 0)
		return;

	// Iterate through each point mass
	for (int i = 0; i < m_Data.m_NoElements; i++)
	{
		Vec2 force = { 0,0 };
		Vec2 pointPos = m_Data.m_Pos[i];

		// iterate through gravity components & calculate gravity force
		for (int gravComp = 0; gravComp < gravityComponents.GetNoInstances(); gravComp++)
		{
			C_GravityComponentManager::C_Instance inst = C_GravityComponentManager::MakeInstance(gravComp);
			const Vec2 gravPos = gravityComponents.GetPos(inst);
			const F32 gravity = gravityComponents.GetGravity(inst);

			// Do gravity simulation
			const float xDist = gravPos.x - pointPos.x;
			const float yDist = gravPos.y - pointPos.y;
			const float dist2 = (xDist * xDist) + (yDist * yDist);
			if (dist2 > 0)
			{
				const F32 ooDist2 = 1.0f / dist2;
				force.x += (xDist * ooDist2) * gravity;
				force.y += (yDist * ooDist2) * gravity;
			}
		}

		m_Data.m_Accel[i].x += force.x;
		m_Data.m_Accel[i].y += force.y;

	}
}