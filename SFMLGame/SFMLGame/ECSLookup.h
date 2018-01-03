#ifndef ECSLOOKUP_H
#define ECSLOOKUP_H
//
//  ObjHandleLookup.h
//  Coding Prototypes
//
//  Created by Mark Craig on 26/02/2011.
//  Copyright 2011 Lucid Games. All rights reserved.
//
typedef unsigned short U16;
typedef unsigned int U32;

typedef U32 C_EntityHandle;
typedef int C_ComponentId;


struct C_Component
{
	C_EntityHandle	m_OwnerEntity;
};

struct C_Entity
{
	C_Component **	m_Components;
};

// Class for assigning handles to objects and looking up objects from their handle
class C_ECSLookup
{
public:    
				C_ECSLookup(int maxEntities, int noComponentTypes);
                ~C_ECSLookup();
    
	C_EntityHandle	CreateEntity(void);             
    void			FreeEntity(C_EntityHandle hEntity);         
	bool			SetEntityComponent(C_EntityHandle hEntity, C_ComponentId component, C_Component *pComponent);
	C_Component *	GetEntityComponent(C_EntityHandle hEntity, C_ComponentId component) const;   /// Get an object's pointer from its id
    
private:
	C_Component**	m_EntityComponents;			// pointers to objects
    U16 *			m_EntityRefs;		// array of object reference ids

    int			m_MaxEntities;		// maximum no of objects
	int			m_NoComponentTypes;
	U16			m_FreeIndex;		// current free index for calculation
    U16			m_NoEntities;		// current number of objects
};

#endif