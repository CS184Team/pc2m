#ifndef PC2M_FACET_H
#define PC2M_FACET_H

class Edge;
#include "Vertex.h"
#include "Vector.h"

class Facet {
public:
	const Vector & get_circumcenter() const;
	double get_circumradius() const;
	double get_circumradius2() const;
	const Vector & get_normal() const;
	const Vertex &va;
	const Vertex &vb;
	const Vertex &vc;
	Facet(Edge *e, const Vertex &v);
	Facet(const Vertex &va, const Vertex &vb, const Vertex &vc);
	friend std::ostream & operator<<(std::ostream &os, const Facet &f);
	friend std::ostream & operator<<(std::ostream &os, const Facet *f);
private:
	Vector circumcenter;
	double circumradius;
	double circumradius2;
	Vector normal;
	void initialize();
};

#endif