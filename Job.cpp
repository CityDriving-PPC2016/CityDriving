#include "Job.h"

Job::Job(const Job & _job)
{
	max = _job.max;
	jobData = vector<int>(_job.jobData);
}

Job::Job(int nodeCount)
{
	max = 0;
	jobData = vector<int>(nodeCount, 0);
}

Job::Job(char * data, int size)
{
	char* end = data;
	memcpy(&max, end, sizeof(int));
	end += sizeof(int);
	size -= sizeof(int);
	jobData = vector<int>(end, end + (size / sizeof(int)));
}

int Job::LastNode() const
{
	return find(jobData.begin(), jobData.end(), max) - jobData.begin();
}

int Job::operator[](const int index)
{
	return jobData[index];
}

Job Job::operator+(const int node)
{
	jobData[node] = ++max;
	return *this;
}

Job Job::operator+=(const int node)
{
	return this->operator+(node);
}

int Job::data(char *& buffer)
{
	int size = sizeof(int) + jobData.size() * sizeof(int);
	buffer = new char[size];
	char* end = buffer;
	memcpy(end, &max, sizeof(int));
	end += sizeof(int);
	memcpy(end, jobData.data(), jobData.size() * sizeof(int));
	return size;
}
