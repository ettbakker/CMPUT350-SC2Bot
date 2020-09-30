#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart() 
{ 
	std::cout << "hello, World!" << std::endl;
	return; 
}

void BasicSc2Bot::OnStep() 
{ 
	//std::cout << Observation()->GetGameLoop() << std::endl;
	TryBuildSupplyDepot();
	TryBuildBarracks();
	return; 
}

void BasicSc2Bot::OnUnitIdle(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
		case UNIT_TYPEID::TERRAN_COMMANDCENTER:
		{
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
			break;
		}

		case UNIT_TYPEID::TERRAN_SCV: 
		{
			const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
			if (!mineral_target) {
				break;

			}
			Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
			break;
		}

		case UNIT_TYPEID::TERRAN_BARRACKS: 
		{
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
			break;
		}

		case UNIT_TYPEID::TERRAN_MARINE: 
		{
			const GameInfo& game_info = Observation()->GetGameInfo();
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
			break;
		}

		default:
		{
			break;
		}
	}
}

