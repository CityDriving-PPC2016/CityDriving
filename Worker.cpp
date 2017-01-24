#include "Constants.h"
#include "Worker.h"
#include "Graph.h"
#include "Job.h"

#include "mpi.h"

void Worker::SendGiveWorkNotification()
{
	char msg = MSG_GIVE_WORK;
	MPI_Send(&msg, 1, MPI_CHAR, 0, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD);
}

void Worker::GiveWorkToWorker(int workerId, Job * job)
{
	char* data;
	int size = job->data(data);
	MPI_Send(data, size, MPI_CHAR, workerId, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
	delete[] data;
}

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

void Worker::ReceiveEndPoint()
{
	MPI_Bcast(&endPoint, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

bool Worker::ReceiveWork()
{
	int size;
	auto status = MPI_Status();
	MPI_Probe(0, TAG_DISPATCH_JOB, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_CHAR, &size);
	if (size == 0)
		return false; // we need to wait

	char* data = new char[size];
	MPI_Recv(data, size, MPI_CHAR, 0, TAG_DISPATCH_JOB, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	Job* job = new Job(data, size);
	jobs.push_back(job);
	delete[] data;
	return true;
}

bool Worker::WaitForWork(int source)
{
	int size;
	auto status = MPI_Status();
	MPI_Probe(source, TAG_DISPATCH_JOB, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_CHAR, &size);

	char* data = new char[size];
	MPI_Recv(data, size, MPI_CHAR, source, TAG_DISPATCH_JOB, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if (size == 1) {
		// message received
		if (data[0] == MSG_NO_WORK) {
			// no work received from another worker, register for waiting to the master
			char msg = MSG_REQUEST_WORK;
			MPI_Send(&msg, 1, MPI_CHAR, 0, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD);
			delete[] data;
			return WaitForWork();
		}
		else if (data[0] == MSG_STOP) {
			return false;
		}
		else if (data[0] > 0) {
			// reroute to another worker
			char msg = MSG_REQUEST_WORK;
			MPI_Send(&msg, 1, MPI_CHAR, data[0], TAG_DEFER_JOB, MPI_COMM_WORLD);
			bool result = WaitForWork(data[0]);
			delete[] data;
			return result;
		}
		else
			throw "unrecognized message received in worker";
	}
	else {
		Job* job = new Job(data, size);
		jobs.push_back(job);
	}
	delete[] data;
	return true;
}

void Worker::FindRoutes()
{
	char msg = MSG_RESULTS;
	MPI_Send(&msg, 1, MPI_CHAR, 0, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD);
}
