#ifndef PC2M_MESHER_H
#define PC2M_MESHER_H

#include <vector>
#include "Vertex.h"
#include "Cloud.h"
#include "Edge.h"
#include "VoxelArray.h"
#include "Facet.h"
#include <utility>
#include <unordered_map>

struct pair_hash {
    inline std::size_t operator()(const std::pair<int, int> & v) const {
        return v.first * 31 + v.second;
    }
};

class Mesher {
public:
	double radius;
	Cloud cloud;
	~Mesher();
	Mesher(Cloud cloud, double radius);
	void constructMesh();
	void expandTriangulation();
	bool findSeedTriangle();
	Vertex * findCandidate(Edge *edge);
	std::vector<Edge *> edge_front;
	VoxelArray *voxelArray;
	std::unordered_map<std::pair<int, int>, Edge *, pair_hash > edges;
	std::vector<Facet *> facets;
};

#endif