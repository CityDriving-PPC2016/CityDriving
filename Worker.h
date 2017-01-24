#pragma once

#ifndef WORKER_H_DEF
#define WORKER_H_DEF

#include<vector>

class Graph;
struct Job;

using namespace std;

class Worker {
private:
	Graph* graph;
	vector<Job*> jobs;

	void SendGiveWorkNotification();
	void GiveWorkToWorker(int workerId, Job* job);
public:
	void ReceiveGraph();
	bool ReceiveWork();
	bool WaitForWork(int source = 0);
	void FindRoutes();

};

#endif // !WORKER_H_DEF
