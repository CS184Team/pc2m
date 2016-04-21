#include "VoxelArray.h"
#include <stdlib.h>
#include <algorithm>
#include <math.h>

VoxelArray::~VoxelArray() {
	free(low);
	free(high);
}

VoxelArray::VoxelArray(const Cloud &cloud, double radius) : cloud(cloud) {
	#ifdef TEST_DEBUG
	std::cout << "[VoxelArray] Constructing a VoxelArray" << std::endl;
	#endif
	this->radius = radius;
	this->diameter = radius * 2;
	double max = std::numeric_limits<double>::max();
	low = new Vector(max);
	high = new Vector(-max);
	for (auto vertex : cloud) {
		if (vertex->x() < low->x) {
			low->x = vertex->x();
		}
		if (vertex->y() < low->y) {
			low->y = vertex->y();
		}
		if (vertex->z() < low->z) {
			low->z = vertex->z();
		}
		if (vertex->x() > high->x) {
			high->x = vertex->x();
		}
		if (vertex->y() > high->y) {
			high->y = vertex->y();
		}
		if (vertex->z() > high->z) {
			high->z = vertex->z();
		}
	}
	Vector diagonal = *high - *low;

	x_len = (int) ceil(diagonal.x / diameter);
	y_len = (int) ceil(diagonal.y / diameter);
	z_len = (int) ceil(diagonal.z / diameter);

	int num_voxels = x_len * y_len * z_len;

	#ifdef TEST_DEBUG
	std::cout << "[VoxelArray] x_len, y_len, z_len = " << x_len << ", " << y_len << ", " << z_len << std::endl;
	#endif

	for (int i = 0; i < x_len * y_len * z_len; ++i) {
		voxels.push_back(Cloud());
	}

	#ifdef TEST_DEBUG
	int i = 0;
	int i_mod = (int) (cloud.size() / 20.0);
	std::cout << "[VoxelArray] Assigning " << cloud.size() << " samples to " << voxels.size() << " cells" << std::endl;
	#endif

	for (auto vertex : cloud) {
		#ifdef TEST_DEBUG
		if (i % i_mod == 0) {
			std::cout << "[VoxelArray] " << (int) (i * 100.0 / cloud.size()) << "%" << " " << i << std::endl;
		}
		++i;
		#endif
		int flat_index = get_flat_index(vertex->position);
		#ifdef TEST_DEBUG
		if (flat_index >= voxels.size()) {
			std::cout << "[VoxelArray] " << flat_index << " out of bounds (" << cloud.size() << ")" << std::endl;
		}
		#endif
		#ifdef TEST_DEBUG
		// std::cout << "[VoxelArray] voxels[" << flat_index << "] = " << voxels[flat_index] << std::endl;
		#endif
		voxels[flat_index].push_back(vertex);
	}

	#ifdef TEST_DEBUG
	std::cout << "[VoxelArray] Done constructing the VoxelArray object" << std::endl;
	#endif
}

void VoxelArray::get_indices(const Vector &vector, int &xi, int &yi, int &zi) const {
	Vector indexVector = (vector - *low) / diameter;
	#ifdef TEST_DEBUG
	// std::cout << "[VoxelArray] indexVector = " << indexVector << std::endl;
	#endif
	xi = (int) indexVector.x;
	yi = (int) indexVector.y;
	zi = (int) indexVector.z;
}

int VoxelArray::get_flat_index(const Vector &vector) const {
	int xi, yi, zi;
	get_indices(vector, xi, yi, zi);
	#ifdef TEST_DEBUG
	// std::cout << "[VoxelArray] xi, yi, zi = " << xi << ", " << yi << ", " << zi << std::endl;
	#endif
	return get_flat_index(xi, yi, zi);
}

int VoxelArray::get_flat_index(int xi, int yi, int zi) const {
	return zi * (x_len * y_len) + yi * x_len + xi;
}

Cloud VoxelArray::get_neighboring_vertices(const Vector &vector) const {
	int xi, yi, zi;
	get_indices(vector, xi, yi, zi);

	Cloud neighbors;

	for (int xj = std::max(0, xi - 1); xj <= std::min(xi + 1, x_len - 1); ++xj) {
		for (int yj = std::max(0, yi - 1); yj <= std::min(yi + 1, y_len - 1); ++yj) {
			for (int zj = std::max(0, zi - 1); zj <= std::min(zi + 1, z_len - 1); ++zj) {
				int flat_index = get_flat_index(xj, yj, zj);
				const Cloud &voxel = voxels[flat_index];
				for (auto vertex : voxel) {
					neighbors.push_back(vertex);
				}
			}
		}
	}

	std::sort(neighbors.begin(), neighbors.end(), [this, &vector](Vertex *a, Vertex *b) {
		return (a->position - vector).norm2() < (b->position - vector).norm2();
	});
	
	return neighbors;
}