#include "Mesher.h"
#include <math.h>
#include <cassert>

Mesher::~Mesher() {
	delete voxelArray;
}

Mesher::Mesher(Cloud cloud, double radius) : cloud(cloud) {
	#ifdef TEST_DEBUG
	std::cout << "[Mesher] Constructing a Mesher object" << std::endl;
	#endif
	this->radius = radius;
	voxelArray = new VoxelArray(cloud, radius);
}

void Mesher::constructMesh() {
    // if (edge_front.empty()) {
	seed_triangle_index = 0;
	while (true) {
		if (!findSeedTriangle()) {
			printf("[Mesher] Could not find a seed triangle. Exiting.\n");
			return;
		} else {
			#ifdef TEST_DEBUG
			printf("[Mesher] Found a seed triangle. Now looking to expand triangulation.\n");
			for (Edge * e : edge_front) {
				std::cout << "[Mesher] e in edge_front = " << e << std::endl;
			}
			#endif
			expandTriangulation();
		}
	}
}

bool Mesher::findSeedTriangle() {
	#ifdef TEST_DEBUG
	std::cout << "[Mesher] Finding the seed triangle" << std::endl;
	#endif
	for (; seed_triangle_index < cloud.size(); ++seed_triangle_index) {
		auto p = cloud[seed_triangle_index];
		Cloud neighbors = voxelArray->get_neighboring_vertices(p->position);
		for (auto q : neighbors) {
			if (p == q) {
				continue;
			}
			for (auto s : neighbors) {
				if (p == s || q == s) {
					continue;
				}
				Vector pq = q->position - p->position;
				Vector ps = s->position - p->position;
				Vector n = cross(pq, ps);
				double d1 = dot(n, p->normal);
				double d2 = dot(n, q->normal);
				double d3 = dot(n, s->normal);
				if (d1 < 0.0 || d2 < 0.0 || d3 < 0.0) {
					continue;
				}
				Facet *f = new Facet(*p, *q, *s);
				Vector c = f->get_circumcenter();
				double r2 = f->get_circumradius2();
				c = c + sqrt(radius * radius - r2) * n;
				bool empty_ball_config = true;
				for (auto v : neighbors) {
					if ((v->position - c).norm2() < r2) {
						empty_ball_config = false;
						break;
					}
				}
				// empty_ball_config = true;
				if (empty_ball_config) {
					Edge *ea = new Edge(*p, *q, f, NULL);
					Edge *eb = new Edge(*q, *s, f, NULL);
					Edge *ec = new Edge(*s, *p, f, NULL);
					edges.insert({ea->get_index(), ea});
					edges.insert({eb->get_index(), eb});
					edges.insert({ec->get_index(), ec});
					edge_front.push_back(ea);
					edge_front.push_back(eb);
					edge_front.push_back(ec);
					facets.push_back(f);
					++seed_triangle_index;
					// return f;
					return true;
				} else {
					delete f;
				}
			}
		}
	}
	return false;
}

void Mesher::expandTriangulation() {
	#ifdef TEST_DEBUG
	std::cout << "[Mesher] Expanding the triangulation" << std::endl;
	#endif
	while (!edge_front.empty()) {
		Edge *e = edge_front.back();
		edge_front.pop_back();
		#ifdef TEST_DEBUG
		std::cout << "[Mesher] Expanding on edge " << e << std::endl;
		#endif
		if (e->is_boundary || e->is_inner) {
			continue;
		}
		Vertex *v = findCandidate(e);
		if (v == NULL) {
			e->is_boundary = true;
			continue;
		}
		// We can always add this facet.
		// We know it's a new facet because, otherwise, e->is_inner would be true.
		Facet *f = new Facet(e, *v);
		facets.push_back(f);

		int v1, v2;
		v1 = e->va.index;
		v2 = v->index;
		std::pair<int, int> edge_index;

		if (v1 > v2) {
			std::swap(v1, v2);
		}
		edge_index = std::pair<int, int>(v1, v2);
		auto iter = edges.find(edge_index);
		bool has_two_adjacent_faces = iter != edges.end();
		if (has_two_adjacent_faces) {
			if (!iter->second->is_inner) {
				iter->second->is_inner = true;
				iter->second->fb = f;
			}
		} else {
			Edge *es = new Edge(e->va, *v, f, NULL);
			edge_front.push_back(es);
			edges.insert({edge_index, es});
		}

		v1 = e->vb.index;
		v2 = v->index;
		if (v1 > v2) {
			std::swap(v1, v2);
		}
		edge_index = std::pair<int, int>(v1, v2);
		iter = edges.find(edge_index);
		has_two_adjacent_faces = iter != edges.end();
		if (has_two_adjacent_faces) {
			if (!iter->second->is_inner) {
				iter->second->is_inner = true;
				iter->second->fb = f;
			}
		} else {
			Edge *et = new Edge(e->vb, *v, f, NULL);
			edge_front.push_back(et);
			edges.insert({edge_index, et});
		}
	}
}

Vertex * Mesher::findCandidate(Edge * e) {
	#ifdef TEST_DEBUG
	std::cout << "[Mesher] Finding a candidate for e " << e << std::endl;
	#endif
	Facet *fa = e->fa;
	#ifdef TEST_DEBUG
	std::cout << "[Mesher] e->fa = " << fa << std::endl;
	#endif
	Vector c = fa->get_circumcenter();
	double r2 = fa->get_circumradius2();
	c = c + sqrt(radius * radius - r2) * fa->get_normal();

	Vector m = e->midpoint();
	double r_prime = (m - c).norm() + radius;
	double t_min = 2 * M_PI;
	Cloud n2r = voxelArray->get_neighboring_vertices(m);
	Cloud nrp = n2r;
	std::remove_if(nrp.begin(), nrp.end(), [m, r_prime](Vertex *v) {
		return (v->position - m).norm() > r_prime;
	});
	Vertex *candidate = NULL;

	for (Vertex *v : nrp) {
		// Don't use v if it is an inner vertex or it belongs to e
		if (v->is_inner || &(e->va) == v || &(e->vb) == v) {
			#ifdef TEST_DEBUG
			std::cout << "[Mesher] " << v << " is either inner or it belongs to e!" << std::endl;
			#endif
			continue;
		}
		// Don't use v if it is not compatible with e (not same orientation)
		// page 157, first paragraph (e.target - e.source) x (v - e.source)
		Vector x = cross(e->vb.position - e->va.position, v->position - e->va.position).unit();
		double d1 = dot(x, v->normal);
		double d2 = dot(x, e->va.normal);
		double d3 = dot(x, e->vb.normal);
		bool compatible = d1 > 0.0 && d2 > 0.0 && d3 > 0.0;
		if (!compatible) {
			#ifdef TEST_DEBUG
			std::cout << "[Mesher] " << v << " is not compatible with " << e << "!" << std::endl;
			#endif
			continue;
		}

		// Compute the circumsphere of c_new
		Facet newFacet(e, *v);
		Vector c_new = newFacet.get_circumcenter();
		double newR2 = newFacet.get_circumradius2();
		double newR = newFacet.get_circumradius();
		double oldR2 = fa->get_circumradius2();

		// If c_new does not exist, continue
		// if (newR2 > oldR2) {
		// 	#ifdef TEST_DEBUG
		// 	std::cout << "[Mesher] c_new does not exist!" << std::endl;
		// 	#endif
		// 	continue;
		// }

		// Compute the angle theta between both circumcenters and the midpoint
		Vector vecA = c - m;
		Vector vecB = c_new - m;
		Vector crossAB = cross(vecA, vecB);
		double theta = angle_between(vecA, vecB);
		if (dot(crossAB, e->vb.position - e->va.position) < 0) {
			theta = 2.0 * M_PI - theta;
		}

		// Make sure that v is first vertex hit by ball
		if (theta < t_min) {
			// Make sure e and v are in and empty ball configuration 
			bool empty_ball_config = true;
			for (Vertex *n : n2r) {
				if ((n->position - c_new).norm() > newR) {
					empty_ball_config = false;
					break;
				}
			}
			if (empty_ball_config) {
				candidate = v;
				t_min = theta;
			} else {
				#ifdef TEST_DEBUG
				std::cout << "[Mesher] " << e << " and " << v << " are not in an empty ball configuration!" << std::endl;
				#endif
			}
		} else {
			#ifdef TEST_DEBUG
			std::cout << "[Mesher] theta < t_min! " << theta << " < " << t_min << std::endl;
			#endif
		}
	}

	#ifdef TEST_DEBUG
	if (candidate == NULL) {
		std::cout << "[Mesher] Could not find candidate for " << e << std::endl;
	} else {
		std::cout << "[Mesher] Found candidate " << candidate << " for " << e << std::endl;
	}
	#endif
	return candidate;
}