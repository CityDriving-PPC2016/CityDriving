#include "Graph.h"

#include <sstream>
#include <iostream>

Graph::Graph(int vertexCount)
{
	size = vertexCount;
	pmHead = 0;
	adjacencyMatrix = unique_ptr<unique_ptr<bool[]>[]>(new unique_ptr<bool[]>[size]);
	for (int i = 0; i < size; i++)
	{
		adjacencyMatrix[i] = unique_ptr<bool[]>(new bool[size]);
		std::fill(&adjacencyMatrix[i][0], &adjacencyMatrix[i][size], false);
	}
}

Graph::Graph(shared_ptr<char> data)
{
	char* end = data.get();
	memcpy(&size, end, sizeof(int));
	end += sizeof(int);

	memcpy(&pmHead, end, sizeof(int));
	end += sizeof(int);

	for (int i = 0; i < size; i++) {
		string l = string(end);
		pointMap[l] = i;
		end += l.length() + 1;
	}

	adjacencyMatrix = unique_ptr<unique_ptr<bool[]>[]>(new unique_ptr<bool[]>[size]);
	for (int i = 0; i < size; i++) {
		adjacencyMatrix[i] = unique_ptr<bool[]>(new bool[size]);
		memcpy(adjacencyMatrix[i].get(), end, size);
		end += size;
	}
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

vector<string> Graph::orderedLabels()
{
	vector<string> labels(size);
	map<string, int>::iterator it = pointMap.begin();
	for (; it != pointMap.end(); it++) {
		labels[it->second] = it->first;
	}
	return labels;
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

vector<string> Graph::GetLabels()
{
	return orderedLabels();
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

int Graph::data(shared_ptr<char>& buffer)
{
	vector<string> labels = orderedLabels();
	int lSize = 0;
	for (int i = 0; i < size; i++) {
		lSize += labels[i].length();
	}

	int dataSize = size*size + 8 + lSize + size;

	buffer = shared_ptr<char>(new char[dataSize]);
	char* end = buffer.get();
	memcpy(end, &size, sizeof(int));
	end += sizeof(int);
	memcpy(end, &pmHead, sizeof(int));
	end += sizeof(int);
	for (int i = 0; i < size; i++) {
		memcpy(end, labels[i].data(), labels[i].length());
		end += labels[i].length();
		end[0] = 0;
		end++;
	}

	for (int i = 0; i < size; i++) {
		memcpy(end, adjacencyMatrix[i].get(), size);
		end += size;
	}

	return dataSize;
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
	vector<string> labels = orderedLabels();

	cout << endl << "\t";
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
