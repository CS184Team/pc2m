#ifndef PC2M_EDGE_H
#define PC2M_EDGE_H

#include "Vertex.h"
#include "Facet.h"
#include <utility>

class Edge {
public:
	Vertex &va;
	Vertex &vb;
	Facet *fa;
	Facet *fb;
	Edge(Vertex &a, Vertex &b, Facet *o, Facet *n);
	Vector midpoint() const;
	bool is_inner;
	bool is_boundary;
	std::pair<int, int> get_index() const;
	friend std::ostream & operator<<(std::ostream &os, const Edge &e);
	friend std::ostream & operator<<(std::ostream &os, const Edge *e);
};

#endif