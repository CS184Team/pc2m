#ifndef PC2M_VECTOR_H
#define PC2M_VECTOR_H

#include <iostream>

class Vector {
public:
	Vector();
	Vector(double v);
	Vector(double x, double y, double z);
	double x;
	double y;
	double z;
	Vector operator-(const Vector &other) const;
	Vector operator+(const Vector &other) const;
	Vector operator/(double k) const;
	friend Vector operator*(double k, const Vector &vec);
	friend Vector operator*(const Vector &vec, double k);
	friend Vector cross(const Vector &a, const Vector &b);
	friend double dot(const Vector &a, const Vector &b);
	friend double angle_between(const Vector &a, const Vector &b);
	double norm2() const;
	double norm() const;
	friend std::ostream& operator<<(std::ostream& os, const Vector &v);
	friend std::ostream& operator<<(std::ostream& os, const Vector *v);
};

#endif