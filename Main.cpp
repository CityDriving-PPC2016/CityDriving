#include "Main.h"

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int rank;

	launchDebugger();

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) {
		// Master
	}
	else {
		// Slaves
	}


	MPI_Finalize();
	return 0;
}