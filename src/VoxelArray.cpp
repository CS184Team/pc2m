#include "VoxelArray.h"
#include <stdlib.h>
#include <algorithm>
#include <math.h>

VoxelArray::~VoxelArray() {
	free(low);
	free(high);
}

VoxelArray::VoxelArray(const Cloud &cloud, double radius) : cloud(cloud) {
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
	Vector diagonal = high - low;

	x_len = (int) ceil(diagonal.x / diameter);
	y_len = (int) ceil(diagonal.y / diameter);
	z_len = (int) ceil(diagonal.z / diameter);

	for (auto vertex : cloud) {
		voxels[get_flat_index(vertex->position)].push_back(vertex);
	}
}

void VoxelArray::get_indices(const Vector &vector, int xi, int yi, int zi) const {
	Vector indexVector = (vector - *low) / diameter;
	xi = (int) indexVector.x;
	yi = (int) indexVector.y;
	zi = (int) indexVector.z;
}

int VoxelArray::get_flat_index(const Vector &vector) const {
	int xi, yi, zi;
	get_indices(vector, xi, yi, zi);
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