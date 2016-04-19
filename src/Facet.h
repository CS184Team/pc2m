#ifndef PC2M_FACET_H
#define PC2M_FACET_H

class Edge;
#include "Vertex.h"
#include "Vector.h"

class Facet {
public:
	Vector get_circumcenter() const;
	double get_circumradius() const;
	double get_circumradius2() const;
	const Vertex &va;
	const Vertex &vb;
	const Vertex &vc;
	Edge *ea;
	Edge *eb;
	Edge *ec;
	Facet(const Vertex &va, const Vertex &vb, const Vertex &vc, Edge *ea, Edge *eb, Edge *ec);
	Facet(Edge *e, const Vertex &v);
private:
	Vector circumcenter;
	double circumradius;
	double circumradius2;
	void initialize();
};

#endif