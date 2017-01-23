#include "Job.h"

Job::Job(int nodeCount)
{
	max = 0;
	data = vector<int>(nodeCount, 0);
}

int Job::LastNode()
{
	return max;
}

int Job::operator[](const int index)
{
	return data[index];
}

Job Job::operator+(const int node)
{
	data[node] = ++max;
	return *this;
}

Job Job::operator+=(const int node)
{
	return this->operator+(node);
}

Job Job::copy()
{
	Job job = Job(data.size());

	job.max = max;
	job.data = vector<int>(data);

	return job;
}
