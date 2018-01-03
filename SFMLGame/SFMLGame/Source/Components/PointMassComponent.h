#ifndef POINTMASSCOMPONENT_H
#define POINTMASSCOMPONENT_H

#include "Types.h"
#include "EntityManager.h"
#include <map>

class C_GravityComponentManager;

class C_PointMassComponentManager
{
	// Types
private:
	struct C_InstanceData
	{
		C_InstanceData()
			: m_NoElements(0)
			, m_NoAllocated(0)
			, m_pBuffer(nullptr)
			, m_Entities(nullptr)
			, m_Pos(nullptr)
			, m_Vel(nullptr)
			, m_Accel(nullptr)
			, m_Mass(nullptr)
		{}

		U32		m_NoElements;
		U32		m_NoAllocated;
		void *	m_pBuffer;		// alocated memory buffer

								// arrays of component elements - structure of arrays
		C_Entity *	m_Entities;	// owning entity
		Vec2 *		m_Pos;
		Vec2 *		m_Vel;
		Vec2 *		m_Accel;
		float *		m_Mass;

		static size_t ComponentSize()
		{
			return sizeof(C_Entity) + sizeof(float) + 3 * sizeof(Vec2);
		}
	};

public:
	// Instance Handle
	struct C_Instance
	{
		U32 m_Index;
	};

	// Data members
private:
	C_InstanceData		m_Data;
	C_EntityIndexMap	m_Map;	// maps entities to components - 

										// Methods
public:

	void Allocate(U32 noToAllocate);
	void Destroy(U32 index);

	// should this be private?
	C_Instance MakeInstance(U32 index) { C_Instance instance = { index }; return instance; }

	C_Instance AddInstance()
	{
		// grow data store if needed
		if (m_Data.m_NoElements >= m_Data.m_NoAllocated)
			Allocate(m_Data.m_NoAllocated * 2);

		return MakeInstance(m_Data.m_NoElements++);
	}

	void DestoryInstance(C_Instance instance)
	{
		Destroy(instance.m_Index);
	}


	// Returns the component instance for the specified entity or a nil instance
	// if the entity doesn't have the component.
	C_Instance GetInstance(C_Entity entity)
	{
		auto it = m_Map.find(entity);
		if (it == m_Map.end())
			return MakeInstance(0);

		return MakeInstance(it->second);
	}

	// component accessors - TODO: add the rest
	Vec2 GetPos(C_Instance instance) { return m_Data.m_Pos[instance.m_Index]; }
	void SetPos(C_Instance instance, Vec2 value) { m_Data.m_Pos[instance.m_Index]; }

	// garbage collection sweep
	void GarbageCollect(const C_EntityManager &entityManager);

	// processing methods
	void Simulate(float dT);
	void ApplyGravityForces(const C_GravityComponentManager &gravityComponents);

};

#endif