#include "Main.h"
#include "Debugger.h"
#include "Master.h"
#include "Worker.h"

#include "Graph.h"
#include "mpi.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);


	int rank, worldSize;
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) {
		// Master

		if (argc > 1) {
			char* inputFile = argv[1];
			ifstream input(inputFile);
			freopen(inputFile, "r", stdin);
		}

		Master master;
		//launchDebugger();
		master.ReadGraph();
		master.SetSearchPoints(5, 22);
		master.PrepareJobs(worldSize);

		master.DispatchGraph();
		master.DispatchEndPoint();
		master.DispatchJobs(worldSize);

		master.WaitForResponse();

		master.DisplayResults();
	}
	else {
		// Slaves

		Worker worker;
		worker.ReceiveGraph();
		worker.ReceiveEndPoint();
		if (!worker.ReceiveWork())
			worker.WaitForWork();
		do {
			//launchDebugger();
			worker.FindRoutes();
		} while (worker.WaitForWork());
	}
	MPI_Finalize();
	return 0;
}