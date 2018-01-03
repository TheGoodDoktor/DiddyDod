//
//  ObjHandleLookup.cpp
//  Coding Prototypes
//
//  Created by Mark Craig on 26/02/2011.
//  Copyright 2011 Lucid Games. All rights reserved.
//


#include "ECSLookup.h"
#include <stdio.h>

#define LOGERROR printf

// simple inline util functions
inline U16 GetIndex(C_EntityHandle id){ return id & 65535;}
inline U16 GetRef(C_EntityHandle id){ return id >> 16;}
inline C_EntityHandle MakeEntityHandle(const U16 index, const U16 ref){return index | (ref << 16);}

// constants
const C_EntityHandle kInvalidEntity = 0;
const U16 kInvalidIndex = 0xffff;

// default constructor
C_ECSLookup::C_ECSLookup(int maxEntities, int noComponentTypes)
	: m_MaxEntities(maxEntities)
	, m_NoComponentTypes(noComponentTypes)
	, m_FreeIndex(0)
	, m_NoEntities(0)
{
	m_EntityComponents = new C_Component *[maxEntities * noComponentTypes];
	m_EntityRefs = new U16[maxEntities];
    
    // clear entity component pointers & initialise indices
    for(int i=0;i<maxEntities;i++)
		m_EntityRefs[i] = 1;   // start at 1 because 0 is invalid

	for (int i = 0; i < maxEntities * noComponentTypes; i++)
		m_EntityComponents[i] = NULL;
}

// destructor - clean up
C_ECSLookup::~C_ECSLookup()
{
    // clean up
    delete m_EntityComponents;
    delete m_EntityRefs;
}

// Generate an id for an object pointer
C_EntityHandle C_ECSLookup::CreateEntity(void)
{
	C_EntityHandle entityHandle = MakeEntityHandle(m_FreeIndex, m_EntityRefs[m_FreeIndex]);

    if(m_NoEntities == m_MaxEntities)
    {
        // report error
		LOGERROR("Max no objects (%d) exceeded", m_MaxEntities);
        return kInvalidEntity;
    }
    
    // clear entity component pointers
	for (int i = 0; i < m_NoComponentTypes; i++)
		m_EntityComponents[(m_FreeIndex * m_NoComponentTypes) + i] = NULL;

	m_NoEntities++;
    
    // have we allocated our last item
    if(m_NoEntities == m_MaxEntities)
    {
        m_FreeIndex = kInvalidIndex;   // set to no free index
    }
    else
    {
        // Go to next free index
        while(m_EntityComponents[m_FreeIndex * m_NoComponentTypes])
        {
            m_FreeIndex++;
            
            // Wrap Around
            if(m_FreeIndex == m_MaxEntities)
                m_FreeIndex = 0;
        }
    }
    return entityHandle;
}

void C_ECSLookup::FreeEntity(C_EntityHandle entityHandle)
{
    const U16 index = GetIndex(entityHandle);

    if(m_EntityRefs[index] == GetRef(entityHandle))
    {
        //m_ObjPtrs[index] = NULL;
        // increment ref
		m_EntityRefs[index]++;

		m_NoEntities--;
        
        // set free index if we have none
        if(m_FreeIndex == kInvalidIndex)
            m_FreeIndex = index;
    }
    else
    {
        // Report error
		LOGERROR("Object for handle 0x%x NOT found", entityHandle);
    }
    
 }

bool C_ECSLookup::SetEntityComponent(C_EntityHandle hEntity, C_ComponentId component, C_Component *pComponent)
{
	const U16 index = GetIndex(hEntity);

	if (m_EntityRefs[index] == GetRef(hEntity))
	{
		m_EntityComponents[(index * m_NoComponentTypes) + component] = pComponent;
		return true;
	}

	return false;
}

C_Component *	C_ECSLookup::GetEntityComponent(C_EntityHandle hEntity, C_ComponentId component) const
{
    const U16 index = GetIndex(hEntity);
    
    if(m_EntityRefs[index] == GetRef(hEntity))
        return m_EntityComponents[(index * m_NoComponentTypes) + component];
    
    return NULL;    // object not found
}
