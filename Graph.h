#pragma once

#ifndef GRAPH_H_DEF
#define GRAPH_H_DEF

#include <string>
#include <map>
#include <list>
using namespace std;

class Graph {
protected:
	bool** adjacencyMatrix;
	int size;
	int pmHead;
	map<string, int> pointMap;

	string GetVertexLabel(int r1, int r2);
	int GetVertexIndex(string label, bool getOnly = false);

	void AddEdge(int x, int y);
	void RemoveEdge(int x, int y);
	bool IsAdjacent(int x, int y);
public:
	Graph(int vertexCount);
	~Graph();
	bool IsAdjacent(string label1, string label2);
	void AddEdge(int road, int cross1, int cross2);
	string GetLabel(int road1, int road2);
	int GetIndex(string label);
	list<int> GetAdjacents(int node);
	int Size();

	void Print();
};

#endif