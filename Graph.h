#pragma once

#ifndef GRAPH_H_DEF
#define GRAPH_H_DEF

#include <string>
#include <list>
#include <memory>
using namespace std;

class Graph {
protected:
	unique_ptr<unique_ptr<bool[]>[]> adjacencyMatrix;
	int size;
	void RemoveEdge(int x, int y);
public:
	Graph(int vertexCount);

	Graph(shared_ptr<char> data);

	bool IsAdjacent(int road, int cross);
	void AddEdge(int road, int cross);
	list<int> GetAdjacents(int node);
	int Size();

	int data(shared_ptr<char>& buffer);

	void Print();
};

#endif // !GRAPH_H_DEF