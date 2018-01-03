#include "Util.h"
#include <random>

namespace Util
{
	U32 RandomInRange(int min, int max)
	{
		return min + (rand() % (max - min));
	}
}

