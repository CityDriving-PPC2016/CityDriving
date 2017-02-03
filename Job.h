#pragma once

#ifndef JOB_H_DEF
#define JOB_H_DEF

#include <vector>
#include <memory>
#include <functional>
using namespace std;

struct Job
{
private:
	int max;
	int lastNode;
	vector<int> jobData;
public:
	Job(const Job& job);
	Job(int nodeCount);
	Job(char* data, int size);

	int NodeCount();
	int LastNode() const;

	int operator[] (const int index);

	Job operator+(const int node);
	Job operator+=(const int node);

	int data(shared_ptr<char>& buffer, bool initialize = true);
	int data(char*& buffer, bool initialize = true);

	int Size();

	void Display(int index, vector<string> labels);
};

#endif // !JOB_H_DEF
