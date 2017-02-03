#pragma once

#ifndef MASTER_H_DEF
#define MASTER_H_DEF

#include <string>
#include <vector>
#include <list>
#include <memory>

class Graph;
struct Job;

using namespace std;

class Master {
private:
	unique_ptr<Graph> graph;

	void WriteOutput(char* msg, bool withOutput);

	string startPoint, endPoint;
	vector<shared_ptr<Job>> jobs;
	vector<shared_ptr<Job>> results;
	list<int> waitingWorkers;
	list<int> workersWithJobsToGive;
	int jobsToWaitFor;

	void SendJob(int workerId, Job job);

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

	void DisplayResults();
};

#endif // !MASTER_H_DEF
