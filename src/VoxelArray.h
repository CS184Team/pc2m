#ifndef PC2M_VOXEL_ARRAY_H
#define PC2M_VOXEL_ARRAY_H

#include "AccelerationStructure.h"
#include "Cloud.h"
#include "Vector.h"

class VoxelArray : public AccelerationStructure {
public:
	VoxelArray(const Cloud &cloud, double rho);
	~VoxelArray();
	Cloud get_neighboring_vertices(const Vector &vector) const;
private:
	double radius;
	double diameter;
	const Cloud &cloud;
	Vector *low;
	Vector *high;
	std::vector<Cloud> voxels;
	int x_len;
	int y_len;
	int z_len;

	int get_flat_index(const Vector &vector) const;
	void get_indices(const Vector &vector, int &xi, int &yi, int &zi) const;
	int get_flat_index(int xi, int yi, int zi) const;
};

#endif