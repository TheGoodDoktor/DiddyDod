//
//  ObjHandleLookup.cpp
//  Coding Prototypes
//
//  Created by Mark Craig on 26/02/2011.
//  Copyright 2011 Lucid Games. All rights reserved.
//

#include "Core/Core.h"
#include "ObjHandleLookup.h"
#include <stdio.h>

// simple inline util functions
inline U16 GetIndex(const C_Handle id){ return id & 65535;}
inline U16 GetRef(const C_Handle id){ return id >> 16;}
inline C_Handle MakeObjHandle(const U16 index, const U16 ref){return index | (ref << 16);}

// constants
const C_Handle kInvalidObj = 0;
const U16 kInvalidIndex = 0xffff;

// default constructor
C_ObjHandleLookup::C_ObjHandleLookup(const U32 maxObjects):m_MaxObjects(maxObjects),m_FreeIndex(0),m_NoObjects(0)
{
    m_ObjPtrs = LG_NEW void *[m_MaxObjects];
    m_ObjRefs = LG_NEW U16[m_MaxObjects];
    
    // clear objects pointers & initialise indices
    for(U32 i=0;i<m_MaxObjects;i++)
    {
        m_ObjPtrs[i] = NULL;
        m_ObjRefs[i] = 1;   // start at 1 because 0 is invalid
    }
}

// destructor - clean up
C_ObjHandleLookup::~C_ObjHandleLookup()
{
    // clean up
    SAFE_DELETE_ARY(m_ObjPtrs);
    SAFE_DELETE_ARY(m_ObjRefs);
}

// Generate an id for an object pointer
C_Handle C_ObjHandleLookup::CreateObjHandle(void *pObject)
{
    C_Handle objHandle = MakeObjHandle(m_FreeIndex, m_ObjRefs[m_FreeIndex]);

    if(m_NoObjects == m_MaxObjects)
    {
        // report error
		LOGERROR("Max no objects (%d) exceeded", m_MaxObjects);
        return kInvalidObj;
    }
    
    // assign object pointer
    m_ObjPtrs[m_FreeIndex] = pObject;
    m_NoObjects++;
    
    // have we allocated our last item
    if(m_NoObjects == m_MaxObjects)
    {
        m_FreeIndex = kInvalidIndex;   // set to no free index
    }
    else
    {
        // Go to next free index
        while(m_ObjPtrs[m_FreeIndex])
        {
            m_FreeIndex++;
            
            // Wrap Around
            if(m_FreeIndex == m_MaxObjects)
                m_FreeIndex = 0;
        }
    }
    return objHandle;
}

void C_ObjHandleLookup::FreeObjHandle(const C_Handle objHandle)
{
    const U16 index = GetIndex(objHandle);

    if(m_ObjRefs[index] == GetRef(objHandle))
    {
        m_ObjPtrs[index] = NULL;
        // increment ref
        m_ObjRefs[index]++;

        m_NoObjects--;
        
        // set free index if we have none
        if(m_FreeIndex == kInvalidIndex)
            m_FreeIndex = index;
    }
    else
    {
        // Report error
		LOGERROR("Object for handle 0x%x NOT found",objHandle);
    }
    
 }

void * C_ObjHandleLookup::GetObjectPtr(const C_Handle objHandle) const
{
    const U16 index = GetIndex(objHandle);
    
    if(m_ObjRefs[index] == GetRef(objHandle))
        return m_ObjPtrs[index];
    
    return NULL;    // object not found
}

// Iterate through all items
void C_ObjHandleLookup::VisitObjects(I_ObjVisitor *pVisitor)
{
    // Iterate through array & visit objects
    // Could be faster by changing the way in which new objects are inserted
    for(U32 i=0;i<m_MaxObjects;i++)
    {
        if(m_ObjPtrs[i])
            pVisitor->VisitObj(m_ObjPtrs[i]);
    }
}

