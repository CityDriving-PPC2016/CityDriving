#include "Debugger.h"
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

void Worker::GiveWorkToWorker(int workerId, Job job)
{
	shared_ptr<char> data;
	int size = job.data(data);
	MPI_Send(data.get(), size, MPI_CHAR, workerId, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
}


void Worker::ReceiveGraph()
{
	int dataSize;
	MPI_Bcast(&dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	shared_ptr<char> data(new char[dataSize]);

	MPI_Bcast(data.get(), dataSize, MPI_CHAR, 0, MPI_COMM_WORLD);
	graph = unique_ptr<Graph>(new Graph(data));
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
	if (size == 1) {
		char msg;
		MPI_Recv(&msg, 1, MPI_CHAR, 0, TAG_DISPATCH_JOB, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (msg == MSG_NO_WORK)
			return false; // we need to wait
		else
			throw "Unknown message in Worker::ReceiveWork";
	}

	shared_ptr<char> data(new char[size]);
	MPI_Recv(data.get(), size, MPI_CHAR, 0, TAG_DISPATCH_JOB, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	shared_ptr<Job> job(new Job(data.get(), size));
	jobs.push_back(job);
	return true;
}

bool Worker::WaitForWork(int source)
{
	int size;
	auto status = MPI_Status();
	MPI_Probe(source, TAG_DISPATCH_JOB, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_CHAR, &size);

	shared_ptr<char> dataPtr(new char[size]);
	auto data = dataPtr.get();
	MPI_Recv(data, size, MPI_CHAR, source, TAG_DISPATCH_JOB, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if (size == 1) {
		// message received
		if (data[0] == MSG_NO_WORK) {
			// no work received from another worker, register for waiting to the master
			char msg = MSG_REQUEST_WORK;
			MPI_Send(&msg, 1, MPI_CHAR, 0, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD);
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
			return result;
		}
		else
			throw "unrecognized message received in worker";
	}
	else {
		shared_ptr<Job> job(new Job(data, size));
		jobs.push_back(job);
	}
	return true;
}

void Worker::FindRoutes()
{
	for (auto it = jobs.begin(); it != jobs.end();) {
		if (!ProcessJob(**it))
			it = jobs.erase(it);
		else
			++it;

		MPI_Status status;
		int flag;
		MPI_Iprobe(MPI_ANY_SOURCE, TAG_DEFER_JOB, MPI_COMM_WORLD, &flag, &status);
		if (flag) {
			char msg;
			MPI_Recv(&msg, 1, MPI_CHAR, status.MPI_SOURCE, TAG_DEFER_JOB, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (msg == MSG_REQUEST_WORK) {
				msg = MSG_NO_WORK;
				MPI_Send(&msg, 1, MPI_CHAR, status.MPI_SOURCE, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
			}
		}
		// send message to master if there are to much jobs in queue to defer to other workers.
	}

	auto size = jobs.size();

	char msg = size == 0 ? MSG_NO_RESULTS : MSG_RESULTS;
	MPI_Send(&msg, 1, MPI_CHAR, 0, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD);

	if (size == 0)
		return;

	auto jobSize = (*jobs.begin())->Size();
	size *= jobSize;
	size += sizeof(int);
	shared_ptr<char> data(new char[size]);

	auto end = data.get();

	memcpy(end, &jobSize, sizeof(int));
	end += sizeof(int);

	for each (auto job in jobs)
	{
		job->data(end, false);
		end += jobSize;
	}
	MPI_Send(data.get(), size, MPI_CHAR, 0, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD);
	jobs.clear();
}


bool Worker::ProcessJob(Job job)
{
	int currentNode = job.LastNode();
	if (currentNode == endPoint)
		return true;

	list<int> adj = graph->GetAdjacents(currentNode);

	for each (int node in adj)
	{
		if (job[node] > 0 || !job.CanAccess(currentNode, node))
			continue;

		shared_ptr<Job> newJob(new Job(job));
		*newJob += node;
		for each (int adjadj in adj) {
			newJob->BlockPath(node, adjadj);
		}
		jobs.push_back(newJob);
	}

	return false;
}