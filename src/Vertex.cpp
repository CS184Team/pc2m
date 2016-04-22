#include "Vertex.h"

Vertex::Vertex(const int index, const Vector &position, const Vector &normal) : position(position), normal(normal) {
	this->index = index;
	in_facet = false;
	front_count = 0;
	is_boundary = false;
}

double Vertex::x() const {
	return position.x;
}

double Vertex::y() const {
	return position.y;
}

double Vertex::z() const {
	return position.z;
}

double Vertex::nx() const {
	return normal.x;
}

double Vertex::ny() const {
	return normal.y;
}

double Vertex::nz() const {
	return normal.z;
}

std::ostream& operator<<(std::ostream& os, const Vertex &v) {
	os << v.index;
	return os;
}

std::ostream& operator<<(std::ostream& os, const Vertex *v) {
	if (v == NULL) {
		os << "NULL";
		return os;
	}
	return os << *v;
}

bool Vertex::is_inner_vertex() const {
	return in_facet && !(front_count > 0 || is_boundary);
}