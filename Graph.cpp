#include "Graph.h"

#include <sstream>
#include <iostream>
#include <vector>

Graph::Graph(int vertexCount)
{
	size = vertexCount;
	pmHead = 0;
	adjacencyMatrix = new bool*[size];
	for (int i = 0; i < size; i++)
	{
		adjacencyMatrix[i] = (bool*)calloc(size, sizeof(bool));
	}
}

Graph::~Graph() {
	for (int i = 0; i < size; i++)
	{
		free(adjacencyMatrix[i]);
	}
	delete[] adjacencyMatrix;
}

bool Graph::IsAdjacent(string label1, string label2)
{
	return IsAdjacent(GetVertexIndex(label1, true), GetVertexIndex(label2, true));
}


bool Graph::IsAdjacent(int x, int y) {
	if (x > 0 && x < size && y > 0 && y < size)
		return adjacencyMatrix[x][y];
	else {
		stringstream s = stringstream();
		s << "Index outside bounds: " << x << ", " << y;
		throw s.str().c_str();
	}
}

void Graph::AddEdge(int road, int cross1, int cross2) {
	int startVertex = GetVertexIndex(GetVertexLabel(road, cross1));
	int endVertex = GetVertexIndex(GetVertexLabel(road, cross2));
	AddEdge(startVertex, endVertex);
}

string Graph::GetLabel(int road1, int road2)
{
	return GetVertexLabel(road1, road2);
}

int Graph::GetIndex(string label)
{
	return GetVertexIndex(label, true);
}

list<int> Graph::GetAdjacents(int node)
{
	list<int> adj = list<int>();
	for (int i = 0; i < size; i++) {
		if (adjacencyMatrix[node][i])
			adj.push_back(i);
	}
	return adj;
}

int Graph::Size()
{
	return size;
}

string Graph::GetVertexLabel(int r1, int r2) {
	stringstream s = stringstream();
	if (r1 < r2)
		s << r1 << "," << r2;
	else
		s << r2 << "," << r1;
	return s.str();
}

int Graph::GetVertexIndex(string label, bool getOnly) {
	map<string, int>::iterator pos = pointMap.find(label);
	if (pos != pointMap.end()) {
		return pos->second;
	}
	else if (getOnly)
		return -1;
	else if (pmHead >= size)
		throw "Exceded the vertext count";
	else {
		pointMap[label] = pmHead;
		return pmHead++;
	}
}

void Graph::AddEdge(int x, int y)
{
	if (x < size && y < size && x != y)
		adjacencyMatrix[x][y] = true;
	else if (x == y)
		throw "Cannot create a loop egde.";
	else
	{
		stringstream s = stringstream();
		s << "Index outside bounds: " << x << ", " << y;
		throw s.str().c_str();
	}
}

void Graph::RemoveEdge(int x, int y)
{
	if (x < size && y < size && x != y)
		adjacencyMatrix[x][y] = false;
	else if (x != y) {
		stringstream s = stringstream();
		s << "Index outside bounds: " << x << ", " << y;
		throw s.str().c_str();
	}
}

void Graph::Print()
{
	cout << endl;
	vector<string> labels(size);
	map<string, int>::iterator it = pointMap.begin();
	for (; it != pointMap.end(); it++) {
		labels[it->second] = it->first;
	}
	cout << "\t";
	for (int i = 0; i < size; i++) {
		cout << labels[i] << "\t";
	}
	cout << endl;
	for (int i = 0; i < size; i++) {
		cout << labels[i] << "\t";
		for (int j = 0; j < size; j++) {
			cout << adjacencyMatrix[i][j] << "\t";
		}
		cout << endl;
	}
}
