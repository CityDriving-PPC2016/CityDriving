#pragma once

#ifndef JOB_H_DEF
#define JOB_H_DEF

#include <vector>
using namespace std;

struct Job
{
private:
	int max;
	vector<int> data;
public:
	Job(int nodeCount);

	int LastNode();

	int operator[] (const int index);

	Job operator+(const int node);
	Job operator+=(const int node);

	Job copy();
};

#endif // !JOB_H_DEF
