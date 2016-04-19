#ifndef PC2M_FILE_IO_H
#define PC2M_FILE_IO_H

#include <vector>
#include "Vertex.h"
#include "Cloud.h"
#include "Facet.h"

class FileIO {
public:
	// ifile is a .txt file with this format:
	// x y z nx ny nz
	// ...
	void readTxt(char *ifile, Cloud &vertices);
	void writePly(char *ofile, const Cloud &vertices, const std::vector<Facet *> &facets);
};

#endif