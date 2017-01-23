#pragma once

#ifndef MASTER_H_DEF
#define MASTER_H_DEF

#include <string>
#include <vector>

class Graph;
class Job;

using namespace std;

class Master {
private:
	Graph* graph;

	void WriteOutput(char* msg, bool withOutput);

	string startPoint, endPoint;
	vector<Job*> jobs;
public:
	void ReadGraph(bool withOutput = false);
	void SetSearchPoints(int x1, int x2, int y1, int y2);
	void PrepareJobs(int worldSize);
	void DispatchGraph();
};

#endif // !MASTER_H_DEF
