#pragma once
#if !defined(__JOB_MANAGER__)
#define __JOB_MANAGER__

#include <list>
#include "Thread.hpp"
#include "Utilities/ThreadSafeQueue.hpp"

typedef void(*job_complete_cb)(void *arg);

enum ePriority {
	HIGH_PRIORITY,
	MEDIUM_PRIORITY,
	LOW_PRIORITY,
	NUM_PRIORITIES

};

class Job
{
	friend class JobManager;

private:
	job_complete_cb m_onCompleteCB;
	void *arg;

public:
	virtual void Run() = 0;
};


class JobThread : public Thread
{
private:
	JobManager *job_manager;

	void ProcessAll();

protected:
	virtual void Run();

public:
	JobThread(JobManager *manager);
};


class JobManager
{
	friend class JobThread;

private:
	bool m_running;
	std::list<Thread*> m_threads;

	ThreadSafeQueue<Job*> m_queuedJobs[NUM_PRIORITIES];
	ThreadSafeQueue<Job*> m_finished_jobs;

public:
	void Initialize(int num_threads);
	void AddThreads(int num_threads);

	inline bool IsRunning() const { return m_running; }

	void Shutdown();

	void Update();
	bool RunJob();

	void PostJob(Job *job, job_complete_cb on_complete, void *arg, ePriority priority = MEDIUM_PRIORITY);
};

unsigned int SystemGetCoreCount();

#endif