#include "Worker.h"
#include "Graph.h"
#include "Job.h"

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

void Worker::ReceiveWork()
{
	int size;
	MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	char* data = new char[size];
	MPI_Recv(data, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	Job* job = new Job(data, size);
	jobs.push_back(job);
	delete[] data;
}

void Worker::FindRoutes()
{
}
