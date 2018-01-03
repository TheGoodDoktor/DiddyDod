#ifndef FIREMANAGER_H
#define FIREMANAGER_H

#include "Types.h"

class C_FireManager
{
public:
	struct C_FireData
	{
		U32			m_NoElements;
		U32			m_NoAllocated;
		void *		m_pBuffer;		// alocated memory buffer

		F32	*	m_Heat;
		F32 *	m_Flamability;
		U16 *	m_Left;
		U16 *	m_Right;
		U16 *	m_Below;
	};

	C_FireData	m_Data;


	void	Allocate(U32 noElements);

	void	Simulate(F32 dT);

private:

};

#endif
