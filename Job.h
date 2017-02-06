#pragma once

#ifndef JOB_H_DEF
#define JOB_H_DEF

#include <vector>
#include <memory>
using namespace std;

struct Job
{
private:
	int max;
	int lastNode;
	vector<int> jobData;
	Job(char staticMode);
public:

	static Job MinJob;
	static shared_ptr<Job> MinJobPtr;
	static Job MaxJob;
	static shared_ptr<Job> MaxJobPtr;

	Job(const Job& job);
	Job(int nodeCount);
	Job(char* data, int size);

	int NodeCount();
	int LastNode() const;

	int operator[] (const int index);

	Job operator+(const int node);
	Job operator+=(const int node);

	void BlockPath(int from, int to);
	bool CanAccess(int from, int to);

	int data(shared_ptr<char>& buffer, bool initialize = true);
	int data(char*& buffer, bool initialize = true);

	int Size();

	void Display();
};

#endif // !JOB_H_DEF
