#pragma once

#ifndef WORKER_H_DEF
#define WORKER_H_DEF

class Graph;

class Worker {
private:
	Graph* graph;
public:
	void ReceiveGraph();
};

#endif // !WORKER_H_DEF
