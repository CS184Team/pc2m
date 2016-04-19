#ifndef PC2M_ACCELERATION_STRUCTURE_H
#define PC2M_ACCELERATION_STRUCTURE_H

#include <vector>
#include "Vector.h"
#include "Cloud.h"

class AccelerationStructure {
	virtual Cloud get_neighboring_vertices(const Vector &vector) const = 0;
};

#endif