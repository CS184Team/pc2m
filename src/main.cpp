#include "FileIO.h"
#include "Mesher.h"

int main(int argc, char **argv) {
	if (argc < 4) {
		printf("Usage:\n\t./pc2m <input.txt> <output.txt> <radius>\n");
	}
	char *ifile = argv[1];
	char *ofile = argv[2];
	// double radius = argv[3];
	double radius;
	std::sscanf(argv[3], "%lf", &radius);
	std::vector<Vertex *> vertices;
	FileIO fileIO;
	fileIO.readTxt(ifile, vertices);
	Mesher mesher(vertices, radius);
	mesher.constructMesh();
	const std::vector<Facet *> &facets = mesher.facets;
	fileIO.writePly(ofile, vertices, facets);
}