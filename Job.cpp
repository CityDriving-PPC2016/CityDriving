#include "Job.h"

#include <iostream>

shared_ptr<Job> Job::MinJobPtr = shared_ptr<Job>(new Job('m'));
shared_ptr<Job> Job::MaxJobPtr = shared_ptr<Job>(new Job('M'));
Job Job::MinJob = *Job::MinJobPtr;
Job Job::MaxJob = *Job::MaxJobPtr;

Job::Job(const Job & _job)
{
	max = _job.max;
	lastNode = _job.lastNode;
	jobData = vector<int>(_job.jobData);
}

Job::Job(char staticMode)
{
	if (staticMode == 'm') {
		max = INT_MAX;
	}
	else if (staticMode == 'M') {
		max = 0;
	}
	lastNode = -1;
	jobData = vector<int>(1, 0);
}

Job::Job(int nodeCount)
{
	max = 0;
	lastNode = -1;
	jobData = vector<int>(nodeCount, 0);
}

Job::Job(char* data, int size)
{
	char* end = data;
	memcpy(&max, end, sizeof(int));
	end += sizeof(int);
	memcpy(&lastNode, end, sizeof(int));
	end += sizeof(int);
	size -= 2 * sizeof(int);
	jobData = vector<int>((int*)end, (int*)end + size / sizeof(int));
}

int Job::NodeCount()
{
	return max;
}

int Job::LastNode() const
{
	return lastNode;
}

int Job::operator[](const int index)
{
	return jobData[index];
}

Job Job::operator+(const int node)
{
	jobData[node] = ++max;
	lastNode = node;
	return *this;
}

Job Job::operator+=(const int node)
{
	return this->operator+(node);
}

void Job::BlockPath(int from, int to)
{
	if (jobData[to] > 0) return;
	jobData[to] = -from - 1;
}

bool Job::CanAccess(int from, int to)
{
	if (jobData[to] + 1 + from == 0)
		return false;
	return true;
}

int Job::data(shared_ptr<char>& buffer, bool initialize)
{
	if (initialize)
		buffer = shared_ptr<char>(new char[Size()]);
	auto d = buffer.get();
	return data(d, false);
}

int Job::data(char*& buffer, bool initialize)
{
	int size = Size();
	if (initialize)
		buffer = new char[size];
	char* end = buffer;
	memcpy(end, &max, sizeof(int));
	end += sizeof(int);
	memcpy(end, &lastNode, sizeof(int));
	end += sizeof(int);
	memcpy(end, jobData.data(), jobData.size() * sizeof(int));
	return size;
}

int Job::Size()
{
	return 2 * sizeof(int) + jobData.size() * sizeof(int);
}

void Job::Display()
{
	vector<int> ordered(jobData.size(), -1);
	for (int i = 0; i < jobData.size(); i++)
	{
		if (jobData[i] > 0)
			ordered[jobData[i] - 1] = i;
	}

	bool written = false;
	for (int i = 0; i < ordered.size(); i++)
	{
		if (ordered[i] == -1)
			break;

		if (written)
			cout << " > ";
		cout << ordered[i] + 1;
		written = true;
	}
	cout << endl;
}
