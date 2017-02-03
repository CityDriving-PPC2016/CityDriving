#pragma once

#ifndef GRAPH_H_DEF
#define GRAPH_H_DEF

#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>
using namespace std;

class Graph {
protected:
	unique_ptr<unique_ptr<bool[]>[]> adjacencyMatrix;
	int size;
	int pmHead;
	map<string, int> pointMap;

	string GetVertexLabel(int r1, int r2);
	int GetVertexIndex(string label, bool getOnly = false);

	void AddEdge(int x, int y);
	void RemoveEdge(int x, int y);
	bool IsAdjacent(int x, int y);

	vector<string> orderedLabels();
public:
	Graph(int vertexCount);

	Graph(shared_ptr<char> data);

	bool IsAdjacent(string label1, string label2);
	void AddEdge(int road, int cross1, int cross2);
	string GetLabel(int road1, int road2);
	vector<string> GetLabels();
	int GetIndex(string label);
	list<int> GetAdjacents(int node);
	int Size();

	int data(shared_ptr<char>& buffer);

	void Print();
};

#endif // !GRAPH_H_DEF