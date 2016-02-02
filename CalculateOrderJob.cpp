#include "CalculateOrderJob.hpp"
#include "AntAI\SD5\GameCode\World.hpp"
#include "Utilities\EngineCommon.hpp"
#include "Assert.hpp"


CalculateOrderJob::CalculateOrderJob(Agent* ant, World* world) :
m_ant(ant),
m_world(world)
{
}


CalculateOrderJob::~CalculateOrderJob()
{
}


void CalculateOrderJob::Run()
{
	//FATAL_ASSERT(!m_ant->m_isRunningJob);
//	m_ant->m_isRunningJob = true;
	CalculateOrderForAnt();
	//m_ant->m_isRunningJob = false;
}

OrderCode CalculateOrderJob::CalculateOrderForAnt() {


	m_world->ValidateAntReportCode(m_ant);

	OrderCode resultCode = ORDER_HOLD;

	Agent* ant = m_ant;
	Agent* m_queen = m_world->m_queen;

	EntityType antType = ant->m_type;
	ArenaSquareType& squareType = m_world->m_map[m_world->CalculateIndexForPosition(ant->m_position)].m_tileType;

	if (!ant->m_currentPath.empty() && ant->m_position == ant->m_currentPath.back())
		ant->m_currentPath.pop_back();

	switch (antType) {
	case ENTITY_TYPE_WORKER: {

		bool needToSuicideWorkers = (!ant->m_specialStatus == ENTITY_SPECIAL_STATUS_CARRYING_FOOD) && (m_world->m_colonyStatus == STATUS_TOO_MANY_WORKERS) && (m_world->m_numWorkers > m_world->m_workerKillThreshold);
		if (needToSuicideWorkers) {
			if (ant->m_currentPath.empty()) {
				bool foundEnemy = m_world->FindClosestEnemy(ant);

				if (foundEnemy && !ant->m_currentPath.empty()) {

					if (ant->m_currentPath.size() == 1 && ant->m_currentPath.back().DistanceSquared(ant->m_position) <= 2) {
						ant->m_targetEnemy->m_checkForKill = true;
					}

					const Vec2i nextPathPos = ant->m_currentPath.back();
					resultCode = m_world->ConvertPositionToMovementCode(ant->m_position, nextPathPos);
				}
				else {
					resultCode = m_world->GetMovementCode(ant->m_position, ant);
				}
			}
		}

		if (ant->m_specialStatus != ENTITY_SPECIAL_STATUS_CARRYING_FOOD && squareType == ARENA_SQUARE_TYPE_FOOD && ant->m_currentPath.empty() && (!(ant->m_position == m_queen->m_position))) { // not carrying food already and on food

			squareType = ARENA_SQUARE_TYPE_AIR;
			resultCode = ORDER_TAKE_FOOD;
		}
		else if (ant->m_specialStatus == ENTITY_SPECIAL_STATUS_CARRYING_FOOD) {  // already carrying food

			if (ant->m_position == m_queen->m_position) {
				ant->m_assignedFood = nullptr;
				resultCode = ORDER_DROP_FOOD;
			}
			else {
				if (ant->m_currentPath.empty()) {
					ant->m_currentPath = m_world->m_pathfinder.GetPathToPosition(ant->m_position, m_queen->m_position, m_world, antType, m_world->m_maxCostForPathfinding);

					//RECOVERABLE_ASSERT(!ant->m_currentPath.empty());
					if (ant->m_currentPath.empty()) {
						resultCode = ORDER_HOLD;
						break;
					}

					const Vec2i nextPathPos = ant->m_currentPath.back();
					resultCode = m_world->ConvertPositionToMovementCode(ant->m_position, nextPathPos);

				}
				else { //Has path back to queen
					const Vec2i nextPathPos = ant->m_currentPath.back();
					resultCode = m_world->ConvertPositionToMovementCode(ant->m_position, nextPathPos);
				}
			}
		}
		else { //Can anyone see any food, if so go get it!

			bool canFindFood = m_world->AssignAntToClosestAvailableFood(ant);

			if (canFindFood && (!ant->m_currentPath.empty())) {
				const Vec2i nextPathPos = ant->m_currentPath.back();
				resultCode = m_world->ConvertPositionToMovementCode(ant->m_position, nextPathPos);
			}
			else {
				resultCode = m_world->GetMovementCode(ant->m_position, ant);
			}
		}
		break;
	}

	case ENTITY_TYPE_SCOUT: {
		bool foundLoc = m_world->CalculateScoutOrder(ant);
		if (foundLoc && !ant->m_currentPath.empty()) {
			const Vec2i nextPathPos = ant->m_currentPath.back();
			resultCode = m_world->ConvertPositionToMovementCode(ant->m_position, nextPathPos);
		}
		else
			resultCode = m_world->GetMovementCode(ant->m_position, ant);
		break;
	}

	case ENTITY_TYPE_SOLDIER: {
		bool foundEnemy = m_world->FindClosestEnemy(ant);
		if (foundEnemy && !ant->m_currentPath.empty()) {

			if (ant->m_currentPath.size() == 1 && ant->m_currentPath.back().DistanceSquared(ant->m_position) <= 1.2) {
				ant->m_targetEnemy->m_checkForKill = true;
			}

			const Vec2i nextPathPos = ant->m_currentPath.back();
			resultCode = m_world->ConvertPositionToMovementCode(ant->m_position, nextPathPos);
		}
		else {
			resultCode = m_world->GetMovementCode(ant->m_position, ant);
		}
		break;
	}

	default:
		if (m_world->m_queenSpawns) {
			resultCode = m_world->CalculateQueenOrders();
		}
		break;
	}

	ant->m_lastAction = resultCode;

	if (ant->m_lastAction == ORDER_HOLD && ant->m_type != ENTITY_TYPE_QUEEN) {
		ant->m_currentPath.clear();
		return ORDER_HOLD;
	}

	//DebuggerPrintf("Turn %i: Ant %i (%i %i), a %s about to execute order %s\n", m_turnNumber, ant->m_ID, ant->m_position.x(), ant->m_position.y(), GetEntityName(ant->m_type), GetOrderName(resultCode));
	return resultCode;
}








