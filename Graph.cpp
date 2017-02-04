#include "Graph.h"

#include <sstream>
#include <iostream>

Graph::Graph(int vertexCount)
{
	size = vertexCount;
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

	adjacencyMatrix = unique_ptr<unique_ptr<bool[]>[]>(new unique_ptr<bool[]>[size]);
	for (int i = 0; i < size; i++) {
		adjacencyMatrix[i] = unique_ptr<bool[]>(new bool[size]);
		memcpy(adjacencyMatrix[i].get(), end, size);
		end += size;
	}
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
	int dataSize = size*size + sizeof(int);

	buffer = shared_ptr<char>(new char[dataSize]);
	char* end = buffer.get();
	memcpy(end, &size, sizeof(int));
	end += sizeof(int);

	for (int i = 0; i < size; i++) {
		memcpy(end, adjacencyMatrix[i].get(), size);
		end += size;
	}

	if (end - buffer.get() > dataSize)
		throw "Incorrect buffer size in Graph::data()";

	return dataSize;
}

void Graph::AddEdge(int x, int y)
{
	x--;
	y--;
	if (x >= 0 && x < size &&
		y >= 0 && y < size &&
		x != y)
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
	cout << endl << "\t";
	for (int i = 0; i < size; i++) {
		cout << i + 1 << "\t";
	}
	cout << endl;
	for (int i = 0; i < size; i++) {
		cout << i + 1 << "\t";
		for (int j = 0; j < size; j++) {
			cout << adjacencyMatrix[i][j] << "\t";
		}
		cout << endl;
	}
}
