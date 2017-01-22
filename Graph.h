#pragma once

#ifndef GRAPH_H_DEF
#define GRAPH_H_DEF

#include <string>
#include <map>
using namespace std;

class Graph {
protected:
	bool** adjacencyMatrix;
	int size;
	int pmHead;
	map<string, int> pointMap;

	string GetVertexLabel(int r1, int r2);
	int GetVertexIndex(string label);
public:
	Graph(int vertexCount);
	~Graph();
	bool IsAdjacent(int x, int y);
	void AddEdge(int road, int cross1, int cross2);
	void AddEdge(int x, int y);
	void RemoveEdge(int x, int y);

	void Print();
};

#endif