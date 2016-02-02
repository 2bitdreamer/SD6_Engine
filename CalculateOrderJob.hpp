#pragma once
#include "JobManager.hpp"
#include "AntAI\SD5\GameCode\CommonInterface.hpp"
struct Agent;
class World;

class CalculateOrderJob :
	public Job
{
public:
	CalculateOrderJob(Agent* ant, World* world);
	~CalculateOrderJob();

	void Run();
	OrderCode CalculateOrderForAnt();
	Agent* m_ant;
	World* m_world;
};

