#include "imatistl.h"
#include <stdlib.h>
#include <string.h>

using namespace IMATI_STL;

void usage()
{
	printf("\nCleaner V0.1alpha - by Marco Attene\n------\n");
	printf("Usage: Cleaner inmeshfile.off outmeshfile.off\n");
	printf("  Copies non-degenerate facets from 'inmeshfile.off' to 'inmeshfile_fixed.off'\n");
	printf("\nHIT ENTER TO EXIT.\n");
	getchar();
	exit(0);
}

char *createFilename(const char *iname, const char *subext, const char *newextension)
{
	static char tname[2048], oname[2048];
	strcpy(tname, iname);
	for (int n = strlen(tname) - 1; n>0; n--) if (tname[n] == '.') { tname[n] = '\0'; break; }
	sprintf(oname, "%s%s%s", tname, subext, newextension);
	return oname;
}


int main(int argc, char *argv[])
{
	ImatiSTL::init(); // This is mandatory
	ImatiSTL::app_name = "Cleaner";
	ImatiSTL::app_version = "0.1alpha";
	ImatiSTL::app_year = "2017";
	ImatiSTL::app_authors = "Marco Attene";
	ImatiSTL::app_maillist = "attene@ge.imati.cnr.it";

	if (argc < 3) usage();

	char *out_filename = argv[2];

	TriMesh tin;
	if (tin.load(argv[1]) != 0) ImatiSTL::error("Can't open file.\n");

	Node *n;
	Triangle *t;
	FOREACHVTTRIANGLE((&(tin.T)), t, n) if (t->isExactlyDegenerate()) tin.unlinkTriangle(t);
	tin.removeUnlinkedElements();
	tin.rebuildConnectivity();

	tin.save(out_filename);

	return 0;
}
