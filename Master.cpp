#include "Constants.h"
#include "Master.h"
#include "Graph.h"
#include "Job.h"

#include "mpi.h"

#include <iostream>
#include <list>
#include <algorithm>

void Master::WriteOutput(char* msg, bool withOutput)
{
	if (withOutput)
		cout << msg;
}

void Master::SendJob(int workerId, Job * job)
{
	char* data;
	int size = job->data(data);
	MPI_Send(data, size, MPI_CHAR, workerId, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
	delete[] data;
}

void Master::SendWork(int workerId)
{
	Job* job = *jobs.begin();
	SendJob(workerId, job);
	jobs.erase(jobs.begin() + 1, jobs.end());
}

void Master::RerouteToWorker(int to, int who)
{
	char data = who;
	MPI_Send(&data, 1, MPI_CHAR, to, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
}

void Master::ReadGraph(bool wOut)
{
	int vNum;
	WriteOutput("Number of intersections: ", wOut);
	cin >> vNum;
	graph = new Graph(vNum + 1);

	int road, cross1, cross2, twoWay;
	for (int i = 0; i < vNum; i++) {
		WriteOutput("Input the street number: ", wOut);
		cin >> road;
		WriteOutput("Input the first cross street: ", wOut);
		cin >> cross1;
		WriteOutput("Input the second cross street: ", wOut);
		cin >> cross2;
		WriteOutput("mode (1 for oneway, 2 for bidirectional): ", wOut);
		cin >> twoWay;

		graph->AddEdge(road, cross1, cross2);
		if (twoWay == 2)
			graph->AddEdge(road, cross2, cross1);
	}

	graph->Print();
}

void Master::PrepareJobs(int worldSize)
{
	int workerCount = worldSize - 1;
	jobs = vector<Job*>();
	vector<int> visited = vector<int>(graph->Size(), -2);

	list<int> queue;
	queue.push_back(graph->GetIndex(startPoint));
	visited[graph->GetIndex(startPoint)] = -1;

	while (!queue.empty()) {
		int current = queue.front();
		queue.pop_front();

		if (visited[current] == -1) {
			Job* job = new Job(graph->Size());
			*job += current;
			jobs.push_back(job);
		}
		else {
			int parentId = visited[current];
			auto parent = find_if(jobs.begin(), jobs.end(), [&parentId](Job* job) {
				return job->LastNode() == parentId;
			});
			Job* job = new Job(**parent);
			*job += current;
			jobs.push_back(job);

			// Removes the parent if we find one
			jobs.erase(remove_if(jobs.begin(), jobs.end(), [&parentId](Job* &arg) {
				return arg->LastNode() == parentId;
			}));
		}

		list<int> adj = graph->GetAdjacents(current);

		if (workerCount > jobs.size() + queue.size()) {
			for each (int node in adj)
			{
				if (visited[node] == -2) {
					visited[node] = current;
					queue.push_back(node);
				}
			}
		}
	}

}

void Master::DispatchGraph()
{
	char* data;
	int dataSize = graph->data(data);

	MPI_Bcast(&dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(data, dataSize, MPI_CHAR, 0, MPI_COMM_WORLD);

	delete[] data;
}

void Master::DispatchEndPoint()
{
	int endIndex = graph->GetIndex(endPoint);
	MPI_Bcast(&endIndex, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void Master::DispatchJobs(int worldSize)
{
	jobsToWaitFor = 0;
	int workerCount = worldSize - 1;
	int jobsToDispatch = 0;
	bool worldBigger = false;
	if (workerCount > jobs.size()) {
		jobsToDispatch = jobs.size();
		worldBigger = true;
	}
	else
		jobsToDispatch = workerCount;

	for (int i = 0; i < jobsToDispatch; i++)
	{
		SendJob(i + 1, jobs[i]);
		jobsToWaitFor++;
	}

	if (worldBigger) {
		for (int i = jobsToDispatch; i < workerCount + 1; i++) {
			// send wait for jobs
			MPI_Send(NULL, 0, MPI_CHAR, i, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
			waitingWorkers.push_back(i);
		}
	}
	else {
		jobs = vector<Job*>(jobs.begin() + jobsToDispatch, jobs.end());
	}
}

void Master::WaitForResponse()
{
	while (waitingWorkers.size() && workersWithJobsToGive.size() && jobsToWaitFor > 0) {
		char msg;
		MPI_Status status;
		MPI_Recv(&msg, 1, MPI_CHAR, MPI_ANY_SOURCE, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD, &status);

		switch (msg)
		{
		case MSG_REQUEST_WORK:
			if (jobs.size()) {
				SendWork(status.MPI_SOURCE);
				jobsToWaitFor++;
			}
			else if (workersWithJobsToGive.size()) {
				RerouteToWorker(*workersWithJobsToGive.begin(), status.MPI_SOURCE);
				workersWithJobsToGive.pop_front();
				jobsToWaitFor++;
			}
			else {
				waitingWorkers.push_back(status.MPI_SOURCE);
			}
			break;

		case MSG_GIVE_WORK:
			if (waitingWorkers.size()) {
				RerouteToWorker(status.MPI_SOURCE, *waitingWorkers.begin());
				waitingWorkers.pop_front();
				jobsToWaitFor++;
			}
			else {
				workersWithJobsToGive.push_back(status.MPI_SOURCE);
			}
			break;

		case MSG_RESULTS:
			// add to results list
			jobsToWaitFor--;
			break;
		default:
			throw "Unrecognized message received by master";
			break;
		}
	}

	// Send stop message
	for each (int worker in waitingWorkers)
	{
		char msg = MSG_STOP;
		MPI_Send(&msg, 1, MPI_CHAR, worker, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
	}
}

void Master::SetSearchPoints(int x1, int x2, int y1, int y2)
{
	startPoint = graph->GetLabel(x1, x2);
	endPoint = graph->GetLabel(y1, y2);
}
