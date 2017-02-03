#pragma once

#ifndef WORKER_H_DEF
#define WORKER_H_DEF

#include <list>
#include <memory>

class Graph;
struct Job;

using namespace std;

class Worker {
private:
	unique_ptr<Graph> graph;
	list<shared_ptr<Job>> jobs;
	int endPoint;
	void SendGiveWorkNotification();
	void GiveWorkToWorker(int workerId, Job job);

	bool ProcessJob(Job job);
public:
	void ReceiveGraph();
	void ReceiveEndPoint();
	bool ReceiveWork();
	bool WaitForWork(int source = 0);
	void FindRoutes();
};

#endif // !WORKER_H_DEF
