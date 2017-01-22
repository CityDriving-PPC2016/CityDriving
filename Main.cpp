#include "Main.h"
#include "Debugger.h"

#include "Graph.h"
#include "mpi.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int rank;

	launchDebugger();

	if (argc > 1) {
		char* inputFile = argv[1];
		ifstream input(inputFile);
		freopen(inputFile, "r", stdin);
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) {
		// Master

		int vNum;
		cout << "Number of intersections: ";
		cin >> vNum;
		Graph g = Graph(vNum + 1);
		int edge = 0, cross1, cross2, twoWay;
		for (int i = 0; i < vNum; i++) {
			cout << "Input the street number: ";
			cin >> edge;
			cout << "Input the first cross street: ";
			cin >> cross1;
			cout << "Input the second cross street: ";
			cin >> cross2;
			cout << "mode (1 for oneway, 2 for bidirectional): ";
			cin >> twoWay;

			g.AddEdge(edge, cross1, cross2);
			if (twoWay == 2)
				g.AddEdge(edge, cross2, cross1);
		}
		g.Print();
		cout << "Master: " << rank;
	}
	else {
		// Slaves
		cout << "Slave: " << rank;
	}



	MPI_Finalize();
	return 0;
}