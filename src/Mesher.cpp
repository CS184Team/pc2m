#include "Mesher.h"
#include <math.h>

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
    if (edge_front.empty()) {
    	if (!findSeedTriangle()) {
    		printf("[Mesher] Could not find a seed triangle. Exiting.\n");
    	} else {
    		#ifdef TEST_DEBUG
    		printf("[Mesher] Found a seed triangle. Now looking to expand triangulation.\n");
    		for (Edge * e : edge_front) {
    			std::cout << "[Mesher] e_i in edge_front = " << e << std::endl;
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
	for (auto p : cloud) {
		Cloud neighbors = voxelArray->get_neighboring_vertices(p->position);
		#ifdef TEST_DEBUG
		#endif
		for (auto q : neighbors) {
			for (auto s : neighbors) {
				if (q == s) {
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
				// added to facets
				Facet *f = new Facet(*p, *q, *s, NULL, NULL, NULL);
				Vector c = f->get_circumcenter();
				double r2 = f->get_circumradius2();
				bool empty_ball_config = true;
				for (auto v : neighbors) {
					if ((v->position - c).norm2() < r2) {
						empty_ball_config = false;
						break;
					}
				}
				if (empty_ball_config) {
					Edge *ea = new Edge(*p, *q, f, NULL);
					Edge *eb = new Edge(*p, *s, f, NULL);
					Edge *ec = new Edge(*q, *s, f, NULL);
					edges.insert({ea->get_index(), ea});
					edges.insert({eb->get_index(), eb});
					edges.insert({ec->get_index(), ec});
					edge_front.push_back(ea);
					edge_front.push_back(eb);
					edge_front.push_back(ec);
					facets.push_back(f);
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
	while (!edge_front.empty()) {
		Edge *e = edge_front.back();
		edge_front.pop_back();
		if (e->is_boundary || e->is_inner) {
			continue;
		}
		Vertex *v = findCandidate(e);
		if (v == NULL) {
			e->is_boundary = true;
			continue;
		}
		// added to facets
		Facet *f = new Facet(e, *v);
		facets.push_back(f);

		edge_front.push_back(f->eb);
		edge_front.push_back(f->ec);

		int v1, v2;


		v1 = e->va.index;
		v2 = v->index;
		std::pair<int, int> edge_index;

		if (v1 > v2) {
			std::swap(v1, v2);
		}
		edge_index = std::pair<int, int>(v1, v2);
		auto iter = edges.find(edge_index);
		if (iter == edges.end()) {
			Edge *es = new Edge(e->va, *v, f, NULL);
			edge_front.push_back(es);
			edges.insert({edge_index, es});
		} else {
			iter->second->is_inner = true;
		}

		v1 = e->vb.index;
		v2 = v->index;
		if (v1 > v2) {
			std::swap(v1, v2);
		}
		edge_index = std::pair<int, int>(v1, v2);
		iter = edges.find(edge_index);
		if (iter == edges.end()) {
			Edge *et = new Edge(e->vb, *v, f, NULL);
			edge_front.push_back(et);
			edges.insert({edge_index, et});
		} else {
			iter->second->is_inner = true;
		}
	}
}

Vertex * Mesher::findCandidate(Edge * e) {
	Facet *fa = e->fa;
	Vector c = fa->get_circumcenter();
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
			continue;
		}
		// Don't use v if it is not compatible with e (not same orientation)
		Vector x = cross(e->va.position - e->vb.position, v->position - e->vb.position);
		double d1 = dot(x, v->normal);
		double d2 = dot(x, e->va.normal);
		double d3 = dot(x, e->vb.normal);
		bool compatible = d1 > 0.0 && d2 > 0.0 && d3 > 0.0;
		if (!compatible) {
			continue;
		}

		// Compute the circumsphere of c_new
		Facet newFacet(e, *v);
		Vector c_new = newFacet.get_circumcenter();
		double newR2 = newFacet.get_circumradius2();
		double newR = newFacet.get_circumradius();
		double oldR2 = fa->get_circumradius2();

		// If c_new does not exist, continue
		if (newR2 > oldR2) {
			continue;
		}

		// Compute the angle theta between both circumcenters and the midpoint
		double theta = angle_between(c - m, c_new - m);

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
			}
		}
	}

	return candidate;
}

// int main(int argc, char **argv) {
// 	int a = 3;
// 	int *a_ptr = &a;
// 	int &a_ref = a;
// 	int &a_ref_ref = a_ref;

// 	int eq = &a_ref_ref == a_ptr;
// 	printf("%d\n", eq);
// }