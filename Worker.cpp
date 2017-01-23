#include "Worker.h"
#include "Graph.h"

#include "mpi.h"

void Worker::ReceiveGraph()
{
	char* data;
	int dataSize;

	MPI_Bcast(&dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	data = new char[dataSize];

	MPI_Bcast(data, dataSize, MPI_CHAR, 0, MPI_COMM_WORLD);
	graph = new Graph(data);
	delete[] data;
}
