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
public:
	void ReceiveGraph();
	void ReceiveWork();
	void FindRoutes();

};

#endif // !WORKER_H_DEF
