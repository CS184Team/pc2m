#include "Edge.h"

Edge::Edge(const Vertex &va, const Vertex &vb, Facet *fa, Facet *fb) : va(va), vb(vb) {
	this->fa = fa;
	this->fb = fb;
}

Vector Edge::midpoint() const {
	return 0.5 * (va.position + vb.position);
}

std::pair<int, int> Edge::get_index() const {
	int v1 = va.index;
	int v2 = vb.index;
	if (v1 > v2) {
		std::swap(v1, v2);
	}
	return std::pair<int, int>(v1, v2);
}