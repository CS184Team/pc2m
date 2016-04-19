#ifndef PC2M_VERTEX_H
#define PC2M_VERTEX_H

#include "Vector.h"
#include <iostream>

class Vertex {
public:
	double x() const;
	double y() const;
	double z() const;
	double nx() const;
	double ny() const;
	double nz() const;
	const Vector &position;
	const Vector &normal;
	bool is_inner;
	int index;
	Vertex(const int index, const Vector &position, const Vector &normal);
	friend std::ostream& operator<<(std::ostream &os, const Vertex &vertex);
	friend std::ostream& operator<<(std::ostream &os, const Vertex *vertex);
};

#endif