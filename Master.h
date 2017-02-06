#pragma once

#ifndef MASTER_H_DEF
#define MASTER_H_DEF

#include <string>
#include <list>
#include <memory>

class Graph;
struct Job;

using namespace std;

class Master {
private:
	struct InputModel {
	private:
		int road;
		int cross;
		bool twoWay;
	public:
		InputModel(int road, int cross, bool twoWay);
		int Road() const;
		int Cross() const;
		bool TwoWay() const;
	};

	unique_ptr<Graph> graph;

	void WriteOutput(char* msg, bool withOutput);

	int startPoint, endPoint;
	list<shared_ptr<Job>> jobs;
	//list<shared_ptr<Job>> results;
	shared_ptr<Job> minJob, maxJob;
	int minIdx, maxIdx;
	list<int> waitingWorkers;
	list<int> workersWithJobsToGive;
	int jobsToWaitFor;

	void SendJob(int workerId, Job job);

	void SendWork(int workerId);
	void RerouteToWorker(int to, int who);
	void HandleWorker(int workerId);
public:
	Master();
	void ReadGraph(bool withOutput = false);
	void SetSearchPoints(int start, int end);
	void PrepareJobs(int worldSize);
	void DispatchGraph();
	void DispatchEndPoint();
	void DispatchJobs(int worldSize);

	void WaitForResponse();

	int DisplayResults(shared_ptr<Job> job, int i = 1);
	void DisplayMinMax();
};


#endif // !MASTER_H_DEF
