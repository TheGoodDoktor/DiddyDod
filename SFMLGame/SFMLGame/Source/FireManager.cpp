#include "FireManager.h"
#include <assert.h>
#include <malloc.h>
#include <string.h>

void C_FireManager::Allocate(U32 noToAllocate)
{
	assert(noToAllocate > m_Data.m_NoElements);

	C_FireData newData;
	const U32 bytes = noToAllocate * (U32)C_FireData::ComponentSize();

	// Allocate buffers
	newData.m_pBuffer = malloc(bytes);
	newData.m_NoElements = m_Data.m_NoElements;
	newData.m_NoAllocated = noToAllocate;

	// Set up pointers
	newData.m_Heat = (F32 *)(newData.m_pBuffer);
	newData.m_Flamability = (F32 *)(newData.m_Heat + noToAllocate);
	newData.m_Left = (U16 *)(newData.m_Flamability + noToAllocate);
	newData.m_Right = (U16 *)(newData.m_Left + noToAllocate);
	newData.m_Below = (U16 *)(newData.m_Right + noToAllocate);

	// copy over old data
	if (m_Data.m_NoElements != 0)
	{
		memcpy(newData.m_Heat, m_Data.m_Heat, m_Data.m_NoElements * sizeof(F32));
		memcpy(newData.m_Flamability, m_Data.m_Flamability, m_Data.m_NoElements * sizeof(F32));
		memcpy(newData.m_Left, m_Data.m_Left, m_Data.m_NoElements * sizeof(U16));
		memcpy(newData.m_Right, m_Data.m_Right, m_Data.m_NoElements * sizeof(U16));
		memcpy(newData.m_Below, m_Data.m_Below, m_Data.m_NoElements * sizeof(U16));
	}

	if (m_Data.m_pBuffer != nullptr)
		free(m_Data.m_pBuffer);	// free old buffer

	m_Data = newData;	// swap to new data
}

void C_FireManager::BuildFromString(const char *str, int stride)
{
	const char *pCharPtr = str;
	int index = 0;

	while (*pCharPtr)
	{
		const int xPos = index % stride;
		const int yPos = index / stride;
		F32 flamability = 0;

		switch (*pCharPtr)
		{
		case '#':
			flamability = 0.5f;
			break;
		default:
			flamability = 0;
			break;
		}

		index++;
		pCharPtr++;
	}
}

// Fire Simulation
const F32 kHeatBurnThreshold = 0.4f;
const F32 kAdjacentHeatEffectScale = 0.2f;
const F32 kBelowHeatEffectScale = 0.4f;

#define ScaleHeat(_heat) ((_heat - kHeatBurnThreshold) * (1.0f / (1.0f - kHeatBurnThreshold)))

void C_FireManager::Simulate(F32 dT)
{
	

	for (U32 i = 0; i < m_Data.m_NoElements; i++)
	{
		F32 heat = 0;

		// heat from left
		const U16 left = m_Data.m_Left[i];
		const F32 leftHeat = left != 0xffff ? m_Data.m_Heat[left] : 0.f;
		if (leftHeat > kHeatBurnThreshold)
			heat += ScaleHeat(leftHeat) * kAdjacentHeatEffectScale;

		// heat from right
		const U16 right = m_Data.m_Right[i];
		const F32 rightHeat = right != 0xffff ? m_Data.m_Heat[right] : 0.f;
		if (rightHeat > kHeatBurnThreshold)
			heat += ScaleHeat(rightHeat) * kAdjacentHeatEffectScale;

		// heat from below
		const U16 below = m_Data.m_Below[i];
		const F32 belowHeat = below != 0xffff ? m_Data.m_Heat[below] : 0.f;
		if (belowHeat > kHeatBurnThreshold)
			heat += ScaleHeat(belowHeat) * kBelowHeatEffectScale;

		// Apply heat
		const bool bWasOnFire = m_Data.m_Heat[i] > kHeatBurnThreshold;
		m_Data.m_Heat[i] += heat;
		const bool bIsOnFire = m_Data.m_Heat[i] > kHeatBurnThreshold;
		if (!bWasOnFire && bIsOnFire)
		{
			// TODO: spawn flame
		}

		if (m_Data.m_Heat[i] > 1.0f)
			m_Data.m_Heat[i] = 1.0f;
	}
}