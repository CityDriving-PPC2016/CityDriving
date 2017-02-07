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
		streambuf* oldStream;
		ifstream input;
		if (argc > 1) {
			char* inputFile = argv[1];
			input = ifstream(inputFile);
			oldStream = cin.rdbuf(input.rdbuf());
		}

		Master master;
		master.ReadGraph();
		cin.rdbuf(oldStream);

		int startNode = 22, endNode = 29;
		cout << "Input the start node:";
		cin >> startNode;
		cout << "Input the end node:";
		cin >> endNode;

		master.SetSearchPoints(startNode, endNode);
		master.PrepareJobs(worldSize);

		master.DispatchGraph();
		master.DispatchEndPoint();
		master.DispatchJobs(worldSize);

		master.WaitForResponse();

		master.DisplayMinMax();
	}
	else {
		// Slaves

		Worker worker;
		worker.ReceiveGraph();
		worker.ReceiveEndPoint();
		if (!worker.ReceiveWork())
			if (!worker.WaitForWork()) {
				MPI_Finalize();
				return 0;
			}
		//launchDebugger();
		do {
			worker.FindRoutes();
		} while (worker.WaitForWork());
	}
	MPI_Finalize();
	return 0;
}