#include "Constants.h"
#include "Master.h"
#include "Graph.h"
#include "Job.h"

#include "mpi.h"

#include <iostream>
#include <list>
#include <algorithm>
#include <unordered_set>

void Master::WriteOutput(char* msg, bool withOutput)
{
	if (withOutput)
		cout << msg;
}

void Master::SendJob(int workerId, Job job)
{
	shared_ptr<char> data;
	int size = job.data(data);
	MPI_Send(data.get(), size, MPI_CHAR, workerId, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
}

void Master::SendWork(int workerId)
{
	SendJob(workerId, **jobs.begin());
	jobs.erase(jobs.begin());
}

void Master::RerouteToWorker(int to, int who)
{
	char data = to;
	MPI_Send(&data, 1, MPI_CHAR, who, TAG_DISPATCH_JOB, MPI_COMM_WORLD);
}

void Master::HandleWorker(int workerId)
{
	if (jobs.size()) {
		SendWork(workerId);
		jobsToWaitFor++;
	}
	else if (workersWithJobsToGive.size()) {
		RerouteToWorker(*workersWithJobsToGive.begin(), workerId);
		workersWithJobsToGive.pop_front();
		jobsToWaitFor++;
	}
	else
		waitingWorkers.push_back(workerId);
}

void Master::ReadGraph(bool wOut) {
	list<InputModel> input;
	unordered_set<int> roads;
	int road, cross, twoWay;
	do {
		WriteOutput("Input the start street number: ", wOut);
		cin >> road;
		if (road == 0)
			break;

		WriteOutput("Input the end street number: ", wOut);
		cin >> cross;

		WriteOutput("mode (1 for oneway, 2 for bidirectional): ", wOut);
		cin >> twoWay;

		input.push_back(InputModel(road, cross, twoWay == 2));
		roads.insert(road);
		roads.insert(cross);
	} while (road);
	//set<int> s(roads.begin(), roads.end());
	graph = unique_ptr<Graph>(new Graph(roads.size()));

	for each (auto item in input)
	{
		graph->AddEdge(item.Road(), item.Cross());
		if (item.TwoWay())
			graph->AddEdge(item.Cross(), item.Road());
	}
}

void Master::PrepareJobs(int worldSize)
{
	int workerCount = worldSize - 1;
	vector<int> visited = vector<int>(graph->Size(), -2);
	vector<int> childCount = vector<int>(graph->Size(), 0);

	list<int> queue;
	queue.push_back(startPoint);
	visited[startPoint] = -1;

	while (!queue.empty()) {
		int current = queue.front();
		queue.pop_front();

		int parentId = visited[current];
		if (parentId == -1) {
			shared_ptr<Job> job(new Job(graph->Size()));
			*job += current;
			jobs.push_back(job);
		}
		else {
			auto parent = find_if(jobs.begin(), jobs.end(), [&parentId](shared_ptr<Job> job) {
				return job->LastNode() == parentId;
			});
			shared_ptr<Job> job(new Job(**parent));
			*job += current;

			if (childCount[parentId] > 0)
				childCount[parentId]--;

			// Removes the parent if we find one
			if (childCount[parentId] == 0)
				jobs.erase(parent);

			jobs.push_back(job);
		}


		int expectedSize = jobs.size() + queue.size();
		if (parentId != -1 && childCount[parentId] > 0) expectedSize--;

		if (workerCount > expectedSize) {
			list<int> adj = graph->GetAdjacents(current);
			for each (int node in adj) {
				if (visited[node] == -2) {
					childCount[current]++;
					visited[node] = current;
					queue.push_back(node);
				}
			}
		}
	}

}

void Master::DispatchGraph()
{
	shared_ptr<char> data;
	int dataSize = graph->data(data);

	MPI_Bcast(&dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(data.get(), dataSize, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Master::DispatchEndPoint()
{
	MPI_Bcast(&endPoint, 1, MPI_INT, 0, MPI_COMM_WORLD);
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
		SendJob(i + 1, *jobs[i]);
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
		jobs = vector<shared_ptr<Job>>(jobs.begin() + jobsToDispatch, jobs.end());
	}
}

void Master::WaitForResponse()
{
	while (workersWithJobsToGive.size() || jobsToWaitFor > 0) {
		char msg;
		MPI_Status status;
		MPI_Recv(&msg, 1, MPI_CHAR, MPI_ANY_SOURCE, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD, &status);

		switch (msg)
		{
		case MSG_REQUEST_WORK:
			HandleWorker(status.MPI_SOURCE);
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

		case MSG_NO_RESULTS:
			// add to results list
			jobsToWaitFor--;
			HandleWorker(status.MPI_SOURCE);
			break;

		case MSG_RESULTS: {
			// add to results list
			jobsToWaitFor--;

			int resultsSize;
			auto statusResults = MPI_Status();
			MPI_Probe(status.MPI_SOURCE, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD, &statusResults);
			MPI_Get_count(&statusResults, MPI_CHAR, &resultsSize);

			shared_ptr<char> data(new char[resultsSize]);
			MPI_Recv(data.get(), resultsSize, MPI_CHAR, status.MPI_SOURCE, TAG_MESSAGE_FROM_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int jobSize;
			auto end = data.get();
			memcpy(&jobSize, end, sizeof(int));
			end += sizeof(int);
			resultsSize -= sizeof(int);

			for (int i = 0; i < resultsSize / jobSize; i++) {
				shared_ptr<Job> job(new Job(end, jobSize));
				end += jobSize;
				results.push_back(job);
			}

			HandleWorker(status.MPI_SOURCE);
			break;
		}
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

void Master::DisplayResults()
{
	if (results.size() == 0) {
		cout << "We did not find any route from the start point to the end point.";
		return;
	}

	int minI = 0, maxI = 0, minCount = INT_MAX, maxCount = 0;
	int i = 1;
	//auto labels = graph->GetLabels();
	for each (auto job in results)
	{
		cout << i++ << ". ";
		job->Display();
		if (job->NodeCount() < minCount) {
			minI = i - 1;
			minCount = job->NodeCount();
		}
		if (job->NodeCount() > maxCount) {
			maxI = i - 1;
			maxCount = job->NodeCount();
		}
	}

	cout << endl << "The shortest path is: " << minI << ". It has " << minCount << " units";
	cout << endl << "The longhest path is: " << maxI << ". It has " << maxCount << " units";
}

void Master::SetSearchPoints(int start, int end)
{
	startPoint = start - 1;
	endPoint = end - 1;
}

Master::InputModel::InputModel(int road, int cross, bool twoWay)
{
	this->road = road;
	this->cross = cross;
	this->twoWay = twoWay;
}

int Master::InputModel::Road() const
{
	return road;
}

int Master::InputModel::Cross() const
{
	return cross;
}

bool Master::InputModel::TwoWay() const
{
	return twoWay;
}
