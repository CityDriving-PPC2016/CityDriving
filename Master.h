#pragma once

#ifndef MASTER_H_DEF
#define MASTER_H_DEF

#include <string>
#include <vector>
#include <list>

class Graph;
struct Job;

using namespace std;

class Master {
private:
	Graph* graph;

	void WriteOutput(char* msg, bool withOutput);

	string startPoint, endPoint;
	vector<Job*> jobs;
	list<int> waitingWorkers;
	list<int> workersWithJobsToGive;
	int jobsToWaitFor;

	void SendJob(int workerId, Job* job);

	void SendWork(int workerId);
	void RerouteToWorker(int to, int who);
public:
	void ReadGraph(bool withOutput = false);
	void SetSearchPoints(int x1, int x2, int y1, int y2);
	void PrepareJobs(int worldSize);
	void DispatchGraph();
	void DispatchEndPoint();
	void DispatchJobs(int worldSize);

	void WaitForResponse();
};

#endif // !MASTER_H_DEF
