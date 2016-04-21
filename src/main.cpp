#include "FileIO.h"
#include "Mesher.h"

int main(int argc, char **argv) {
	if (argc < 4) {
		printf("Usage:\n\t%s <input.txt> <output.txt> <radius>\n", argv[0]);
		printf("Use radius = 0.0005 for the Stanford Dragon\n");
		return 1;
	}
	#ifdef TEST_DEBUG
	printf("[main] Executing %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
	#endif
	char *ifile = argv[1];
	char *ofile = argv[2];
	double radius;
	std::sscanf(argv[3], "%lf", &radius);
	std::vector<Vertex *> vertices;
	FileIO fileIO;
	#ifdef TEST_DEBUG
	printf("[main] Reading %s (this may take a while)\n", ifile);
	#endif
	fileIO.readTxt(ifile, vertices);
	Mesher mesher(vertices, radius);
	mesher.constructMesh();
	const std::vector<Facet *> &facets = mesher.facets;
	fileIO.writePly(ofile, vertices, facets);
}