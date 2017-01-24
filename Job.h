#pragma once

#ifndef JOB_H_DEF
#define JOB_H_DEF

#include <vector>
using namespace std;

struct Job
{
private:
	int max;
	vector<int> jobData;
public:
	Job(const Job& job);
	Job(int nodeCount);
	Job(char* data, int size);

	int LastNode() const;

	int operator[] (const int index);

	Job operator+(const int node);
	Job operator+=(const int node);

	int data(char*& buffer);
};

#endif // !JOB_H_DEF
