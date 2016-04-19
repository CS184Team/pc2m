#include "Vector.h"
#include <math.h>

Vector::Vector(double x, double y, double z) : x(x), y(y), z(z) {}

Vector::Vector(double v) : x(v), y(v), z(v) {}

Vector::Vector() {
	Vector(0);
}

Vector Vector::operator-(const Vector &other) const {
	return Vector(x - other.x, y - other.y, z - other.z);
}

Vector Vector::operator+(const Vector &other) const {
	return Vector(x + other.x, y + other.y, z + other.z);
}

Vector Vector::operator/(double k) const {
	return Vector(x / k, y / k, z / k);
}

double Vector::norm2() const {
	return x * x + y * y + z * z;
}

double Vector::norm() const {
	return sqrt(norm2());
}

Vector operator*(double k, const Vector &vec) {
	return Vector(vec.x * k, vec.y * k, vec.z * k);
}

Vector operator*(const Vector &vec, double k) {
	return Vector(vec.x * k, vec.y * k, vec.z * k);
}

Vector cross(const Vector &a, const Vector &b) {
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

double dot(const Vector &a, const Vector &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

double angle_between(const Vector &a, const Vector &b) {
	double d = dot(a, b);
	double na = a.norm();
	double nb = b.norm();
	double cos_theta = d / (na * nb);
	return acos(cos_theta);
}

std::ostream& operator<<(std::ostream& os, const Vector& v) {
	os << v.x << " " << v.y << " " << v.z;
	return os;
}