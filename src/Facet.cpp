#include "Facet.h"
#include "Edge.h"
#include <cstddef>
#include <stdio.h>
#include <math.h>

Facet::Facet(const Vertex &va, const Vertex &vb, const Vertex &vc, Edge *ea, Edge *eb, Edge *ec) : 
	va(va),
	vb(vb),
	vc(vc),
	ea(ea),
	eb(eb),
	ec(ec) {
	initialize();
}

Facet::Facet(Edge *e, const Vertex &v) : va(e->va), vb(e->vb), vc(v) {
	ea = e;
	eb = new Edge(va, vc, e->fa, NULL);
	ec = new Edge(vb, vc, e->fa, NULL);
	initialize();
}

void Facet::initialize() {
	double a = (vb.position - vc.position).norm();
	double b = (va.position - vc.position).norm();
	double c = (va.position - vb.position).norm();
	double a2 = a * a;
	double b2 = b * b;
	double c2 = c * c;
	double alpha = a2 * (b2 + c2 - a2);
	double beta = b2 * (a2 + c2 - b2);
	double gamma = c2 * (a2 + b2 - c2);
	double sum = alpha + beta + gamma;
	double k = 1.0 / sum;
	Vector bary = k * Vector(alpha, beta, gamma);
	circumcenter = bary.x * va.position + bary.y * vb.position + bary.z * vc.position;
	circumradius2 = a2 * b2 * c2 / ((a + b + c) * (b + c - a) * (c + a - b) * (a + b - c));
	circumradius = sqrt(circumradius2);
}

Vector Facet::get_circumcenter() const {
	return circumcenter;
}

double Facet::get_circumradius() const {
	return circumradius;
}

double Facet::get_circumradius2() const {
	return circumradius2;
}

// int main(int argc, char **argv) {
// 	Vector a = Vector(5.0/2, 2.5, 0);
// 	Vector b = Vector(4, 4.0/5, 0);
// 	Vector c = Vector(0.5, 1.0/4, 0);
// 	Vector n = Vector();
// 	Vertex va = Vertex(a, n);
// 	Vertex vb = Vertex(b, n);
// 	Vertex vc = Vertex(c, n);
// 	Edge e(va, vb, NULL, NULL);
// 	Facet facet(va, vb, vc, e, e, e);
// 	Vector center = facet.circumcenter();
// 	printf("%f %f %f\n", center.x, center.y, center.z);
// }