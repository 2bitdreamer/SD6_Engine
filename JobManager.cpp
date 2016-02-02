#include "JobManager.hpp"
#include <sysinfoapi.h>

void JobThread::ProcessAll()
{
	while (job_manager->RunJob()) {}
}

void JobThread::Run()
{
	while (job_manager->IsRunning())
	{
		ProcessAll();
		YieldExecution();
	}

	ProcessAll();
}

JobThread::JobThread(JobManager *manager)
{
	job_manager = manager;
}

void JobManager::Initialize(int num_threads)
{
	m_running = true;
	AddThreads(num_threads);
}

void JobManager::AddThreads(int num_threads)
{

	for (int th = 0; th < num_threads; th++) {
		JobThread *job_thread = new JobThread(this);
		job_thread->Start("job_thread");

		m_threads.push_back(job_thread);
	}
}

void JobManager::Shutdown()
{
	m_running = false;
	while (m_threads.size() > 0) {
		Thread* thread = m_threads.front();
		thread->Join();
		delete thread;

		m_threads.pop_front();
	}

	// finish out finished jobs
	Update();
}

void JobManager::Update()
{
	if (m_threads.empty()) {
		RunJob();
	}

	Job* finished_job;
	while (m_finished_jobs.dequeue(&finished_job)) {
		if (finished_job->m_onCompleteCB != nullptr) {
			finished_job->m_onCompleteCB(finished_job->arg);
		}

		delete finished_job;
	}
}

bool JobManager::RunJob()
{
	Job *job;

	for (int p = 0; p < NUM_PRIORITIES; p++) {
		if (m_queuedJobs[p].dequeue(&job))
		{
			job->Run();
			m_finished_jobs.enqueue(job);
			return true;
		}
	}

	return false;
}

void JobManager::PostJob(Job *job, job_complete_cb on_complete, void *arg, ePriority priority /*= MEDIUM_PRIORITY*/)
{
	job->m_onCompleteCB = on_complete;
	job->arg = arg;

	m_queuedJobs[priority].enqueue(job);
}

unsigned int SystemGetCoreCount()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	return (unsigned int)sysinfo.dwNumberOfProcessors;
}