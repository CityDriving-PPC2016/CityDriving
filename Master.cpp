#include "Master.h"
#include "Graph.h"
#include "Job.h"

#include "mpi.h"

#include <iostream>
#include <list>

void Master::WriteOutput(char* msg, bool withOutput)
{
	if (withOutput)
		cout << msg;
}

void Master::ReadGraph(bool wOut)
{
	int vNum;
	WriteOutput("Number of intersections: ", wOut);
	cin >> vNum;
	graph = new Graph(vNum + 1);

	int road, cross1, cross2, twoWay;
	for (int i = 0; i < vNum; i++) {
		WriteOutput("Input the street number: ", wOut);
		cin >> road;
		WriteOutput("Input the first cross street: ", wOut);
		cin >> cross1;
		WriteOutput("Input the second cross street: ", wOut);
		cin >> cross2;
		WriteOutput("mode (1 for oneway, 2 for bidirectional): ", wOut);
		cin >> twoWay;

		graph->AddEdge(road, cross1, cross2);
		if (twoWay == 2)
			graph->AddEdge(road, cross2, cross1);
	}

	graph->Print();
}

void Master::PrepareJobs(int worldSize)
{
	int workerCount = worldSize - 1;
	jobs = vector<Job*>();
	vector<int> visited = vector<int>(graph->Size(), -1);

	list<int> queue;
	queue.push_back(graph->GetIndex(startPoint));
	visited[graph->GetIndex(startPoint)] = 0;

	while (workerCount > jobs.size() && !queue.empty()) {
		int current = queue.front();
		queue.pop_front();

		if (visited[current] == 0) {
			Job* job = new Job(graph->Size());
			*job += current;
			jobs.push_back(job);
		}
		else {
			Job* job = new Job(*jobs[current]);
			*job += current;
			jobs.push_back(job);
		}

		list<int> adj = graph->GetAdjacents(current);

		for each (int node in adj)
		{
			if (visited[node] == -1) {
				visited[node] = current;
				queue.push_back(node);
			}
		}
	}

}

void Master::DispatchGraph()
{
	char* data;
	int dataSize = graph->data(data);

	MPI_Bcast(&dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(data, dataSize, MPI_CHAR, 0, MPI_COMM_WORLD);

	delete[] data;
}

void Master::DispatchJobs()
{
	for (int i = 0; i < jobs.size(); i++)
	{
		char* data;
		int size = jobs[i]->data(data);
		MPI_Send(&size, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
		MPI_Send(data, size, MPI_CHAR, i + 1, 0, MPI_COMM_WORLD);
		delete[] data;
	}
}

void Master::SetSearchPoints(int x1, int x2, int y1, int y2)
{
	startPoint = graph->GetLabel(x1, x2);
	endPoint = graph->GetLabel(y1, y2);
}
