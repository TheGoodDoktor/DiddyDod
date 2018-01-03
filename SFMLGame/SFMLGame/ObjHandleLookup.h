#ifndef OBJHANDLELOOKUP_H
#define OBJHANDLELOOKUP_H
//
//  ObjHandleLookup.h
//  Coding Prototypes
//
//  Created by Mark Craig on 26/02/2011.
//  Copyright 2011 Lucid Games. All rights reserved.
//
#include "core/core.h"

typedef U32 C_Handle;

class I_ObjVisitor
{
public:
	virtual			~I_ObjVisitor(){}
    virtual void    VisitObj(void *objPtr) const = 0;
};

// Class for assigning handles to objects and looking up objects from their handle
class C_ObjHandleLookup
{
public:    
                C_ObjHandleLookup(const U32 maxObjects);
                ~C_ObjHandleLookup();
    
    C_Handle    CreateObjHandle(void *pObject);             /// Create an object id for a an object pointer
    void        FreeObjHandle(const C_Handle objId);         /// Free an object id
    void *      GetObjectPtr(const C_Handle objId) const;   /// Get an object's pointer from its id
    
    U32         NoObjects(void) const { return m_NoObjects; }   // Return number of objects
    void        VisitObjects(I_ObjVisitor *pVisitor);
    
private:
    void ** m_ObjPtrs;      // pointers to objects
    U16 *   m_ObjRefs;      // array of object reference ids
    U32     m_MaxObjects;   // maximum no of objects
    U16     m_FreeIndex;    // current free index for calculation
    U16     m_NoObjects;    // current number of objects
};

#endif